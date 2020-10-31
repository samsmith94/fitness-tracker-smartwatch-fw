//#define BLE_CUS_TEST

#if defined(BLE_CUS_TEST)
#include "../ble/ble_cus.h"
#else

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#include "gesture/apds9960.h"
//#include "display/st7735.h"
//#include "display/fonts.h"

#include "gesture/apds9960.h"
#include "rtc/calendar.h"

#include "nrf_drv_timer.h"

#include "test_apds9960.h"
#include "nrf_drv_systick.h"

#include "../buzzer/rtttl.h"

#include "../imu/driver/lsm6dsox_tap.h"
#include "../imu/driver/lsm6dsox_fifo_pedo.h"
#include "../imu/driver/lsm6dsox_read_data.h"

#include "../imu/driver/lsm6dsox_multi_conf.h"

#include "display/menu.h"

//#include "nrf_timer.h"

#include "utilities/utility.h"
#endif

#if defined(BLE_CUS_TEST)
#define USER_LED BSP_BOARD_LED_1

void led_write_handler(uint16_t conn_handle, ble_led_service_t *p_led_service, uint8_t led_state);

/**< Battery timer. */
APP_TIMER_DEF(m_battery_timer_id);

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    timers_init();
    leds_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();

    services_init();
    conn_params_init();

    //peer_manager_init();

    // Start execution.
    NRF_LOG_INFO("Custom BLE service started.");
    application_timers_start();

    advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
        if (RTT_GetKey())
        {
            battery_charge_level = atoi(RTT_String);
        }
    }
}

/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_led_service  Instance of LED Service to which the write applies.
 * @param[in] led_state      Written/desired state of the LED.
 */
void led_write_handler(uint16_t conn_handle, ble_led_service_t *p_led_service, uint8_t led_state)
{
    if (led_state)
    {
        bsp_board_led_on(USER_LED);
        NRF_LOG_INFO("Received LED ON!");
    }
    else
    {
        bsp_board_led_off(USER_LED);
        NRF_LOG_INFO("Received LED OFF!");
    }
}
#else

/* Sleep On *******************************************************************/

#define NRF52_ONRAM1_OFFRAM1 POWER_RAM_POWER_S0POWER_On << POWER_RAM_POWER_S0POWER_Pos | POWER_RAM_POWER_S1POWER_On << POWER_RAM_POWER_S1POWER_Pos | POWER_RAM_POWER_S0RETENTION_On << POWER_RAM_POWER_S0RETENTION_Pos | POWER_RAM_POWER_S1RETENTION_On << POWER_RAM_POWER_S1RETENTION_Pos;

#define NRF52_ONRAM1_OFFRAM0 POWER_RAM_POWER_S0POWER_On << POWER_RAM_POWER_S0POWER_Pos | POWER_RAM_POWER_S1POWER_On << POWER_RAM_POWER_S1POWER_Pos | POWER_RAM_POWER_S0RETENTION_Off << POWER_RAM_POWER_S0RETENTION_Pos | POWER_RAM_POWER_S1RETENTION_Off << POWER_RAM_POWER_S1RETENTION_Pos;

#define NRF52_ONRAM0_OFFRAM0 POWER_RAM_POWER_S0POWER_Off << POWER_RAM_POWER_S0POWER_Pos | POWER_RAM_POWER_S1POWER_Off << POWER_RAM_POWER_S1POWER_Pos;

void configure_ram_retention(void)
{
    // Configure nRF52 RAM retention parameters. Set for System On 64kB RAM retention
    NRF_POWER->RAM[0].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[1].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[2].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[3].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[4].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[5].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[6].POWER = NRF52_ONRAM1_OFFRAM0;
    NRF_POWER->RAM[7].POWER = NRF52_ONRAM1_OFFRAM0;
}

#define ENTER_SYSTEM_ON_SLEEP_MODE() \
    {                                \
        __WFE();                     \
        __SEV();                     \
        __WFE();                     \
    }

void wakeup_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("nRF52 wake up.");
}

#define PIN_IN_POWER_DOWN BSP_BUTTON_1

static void pin_in_power_down_gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    err_code = nrf_drv_gpiote_in_init(PIN_IN_POWER_DOWN, &in_config, wakeup_pin_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(PIN_IN_POWER_DOWN, true);
}

/* Stopper/counter ************************************************************/

typedef enum
{
    STOPPER_MODE = 0,
    TIMER_MODE
} stopper_timer_mode_t;

stopper_timer_mode_t stopper_timer_mode;

typedef struct
{
    uint8_t min;
    uint8_t sec;
    uint8_t tenth_of_sec;
} stopper_counter_t;

stopper_counter_t timer;

/* Timer **********************************************************************/
const nrf_drv_timer_t TIMER_TEST = NRF_DRV_TIMER_INSTANCE(0);

uint32_t expiry_time = 50; //5 sec
bool timer_expired = false;
/*
5000 ms = 5 sec
5000/100 = 50
*/

uint32_t timer_event_handler_cnt = 0;

int tenth_of_seconds = 10 * 60; //1 perc
void timer_event_handler(nrf_timer_event_t event_type, void *p_context)
{

    /*
	static uint32_t i;
    uint32_t led_to_invert = ((i++) % LEDS_NUMBER);
    */

    timer_event_handler_cnt++;

    switch (event_type)
    {
    case NRF_TIMER_EVENT_COMPARE0:
        //NRF_LOG_INFO("timer_event_handler called. %d", i);
        NRF_LOG_INFO("timer_event_handler called. %d", timer_event_handler_cnt);

        //bsp_board_led_invert(led_to_invert);

        //time_counter++;
        if (stopper_timer_mode == TIMER_MODE)
        {
            tenth_of_seconds--;
            timer.min = tenth_of_seconds / (60 * 10);
            timer.sec = (tenth_of_seconds - (timer.min * 60 * 10)) / 10;
            timer.tenth_of_sec = tenth_of_seconds - (timer.min * 60 * 10) - (timer.sec * 10);
            NRF_LOG_INFO("Timer: %02d:%02d.%01d\r\n", timer.min, timer.sec, timer.tenth_of_sec);
            if (tenth_of_seconds == 0)
            {
                nrf_drv_timer_disable(&TIMER_TEST);
            }
        }
        else if (stopper_timer_mode == STOPPER_MODE)
        {
            timer.tenth_of_sec++;
            if (timer.tenth_of_sec == 10)
            {
                timer.tenth_of_sec = 0;
                timer.sec++;
                if (timer.sec == 59)
                {
                    timer.sec = 0;
                    timer.min++;
                }
            }
            NRF_LOG_INFO("Stopper: %02d:%02d.%02d\r\n", timer.min, timer.sec, timer.tenth_of_sec);
        }

        break;

    default:
        //Do nothing.
        break;
    }
    /*
    if (timer_event_handler_cnt == expiry_time)
    {
        timer_expired = true;
        nrf_drv_timer_disable(&TIMER_TEST);
        NRF_LOG_INFO("Timer expired.");
        st7735_sleep_in();
        set_display_pwm(0);

        timer_event_handler_cnt = 0;
        //ENTER_SYSTEM_ON_SLEEP_MODE();
        //nrf_pwr_mgmt_run();
    }
    */
}

void start_stopper_timer(nrfx_timer_event_handler_t timer_event_cb, stopper_timer_mode_t mode, uint8_t minutes)
{
    stopper_timer_mode = mode;
    tenth_of_seconds = 10 * 60 * minutes;

    nrf_drv_timer_disable(&TIMER_TEST);
    nrf_drv_timer_uninit(&TIMER_TEST);
    uint32_t err_code;

    /* Timer ******************************************************************/
    uint32_t time_ms = 100; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    err_code = NRF_SUCCESS;

    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_62500Hz;
    err_code = nrf_drv_timer_init(&TIMER_TEST, &timer_cfg, timer_event_cb);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_TEST, time_ms);

    nrf_drv_timer_extended_compare(&TIMER_TEST, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&TIMER_TEST);
}

/******************************************************************************/

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uint32_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    bsp_board_init(BSP_INIT_LEDS);

    /* Display ****************************************************************/

    print_ascii_art();

    st7735_init();
    nrf_delay_ms(20);
    st7735_fill_screen(ST7735_BLACK);

    //st7735_invert_colors(false);
    /*
    draw_widget(heart_widget, 10, 40 - (14 / 2) - 25);
    nrf_delay_ms(1000);
    draw_widget(facebook_widget, 30, 40 - (14 / 2));
    nrf_delay_ms(1000);
*/

    /* Calendar ***************************************************************/
    calendar_init();

    /* Menu *******************************************************************/
    level main_menu, stopper_menu, timer_menu, *current_menu;

    level activity_menu;

    build_menu(&main_menu, &activity_menu, &stopper_menu, render_main_screen);
    build_menu(&stopper_menu, &main_menu, &timer_menu, render_stopper_screen);
    build_menu(&timer_menu, &stopper_menu, &activity_menu, render_timer_screen);

    build_menu(&activity_menu, &timer_menu, &main_menu, render_activity_screen);

    current_menu = &main_menu;
    current_menu->render();

    /* Gesture ****************************************************************/
    apds9960_gesture_received_t gesture_received;

    //new_gesture_init();
    //gpio_init();

    i2c_init();
    gesture_init();

    nrf_delay_ms(100);
    
    lsm6dsox_multi_conf_init();

    //lsm6dsox_fsm_init();

    //lsm6dsox_tap_init();
    //lsm6dsox_read_data_init();

    //lsm6dsox_fifo_pedo_init();

    

    /* Display ****************************************************************/
    init_display_pwm();
    st7735_sleep_out();

    /* Buzzer *****************************************************************/
    buzzer_init();
    test_rtttl_player();

    button_1_init();

    /* Systick ... ************************************************************/
    /* Init systick driver */
    /*nrf_drv_systick_init();
    nrfx_systick_state_t p_state;

    while (1)
    {
        nrf_drv_systick_delay_ms(1000);
        nrfx_systick_get(&p_state);
        NRF_LOG_INFO("systick: %lu", p_state.time);
    }
    */
    /**************************************************************************/
    while (true)
    {
        /*
        gesture_received = apds9960_read_gesture();
        apds9960_gesture_to_uart(gesture_received);

        if (gesture_received == APDS9960_RIGHT)
        {
            prev(&current_menu);
            st7735_sleep_out();
            //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, 5));
            set_display_pwm(5);
            timer_event_handler_cnt = 0;
            //start_keepalive_timer(timer_event_handler);

            start_display_keepalive();
        }
        else if (gesture_received == APDS9960_LEFT)
        {
            next(&current_menu);
            st7735_sleep_out();
            //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, 5));
            set_display_pwm(5);
            timer_event_handler_cnt = 0;
            //start_keepalive_timer(timer_event_handler);

            start_display_keepalive();
        }
        else if (gesture_received == ADPS9960_INVALID)
        {
            //lsm6dsox_read_steps();
            //NRF_LOG_INFO("WHILE");
        }
        nrf_delay_ms(50);
        */

        /**********************************************************************/

        //ez egy kicsit szétcseszi a gesture-t....
        if (RTT_GetKey())
        {
            NRF_LOG_INFO("Received from RTT Viewer: %s, %d", RTT_String, strlen(RTT_String));
            if (strcmp(RTT_String, "TIMER") == 0)
            {
                //NRF_LOG_INFO("Starting timer...");
                start_stopper_timer(timer_event_handler, TIMER_MODE, 2);
            }
            else if (strcmp(RTT_String, "STOPPER") == 0)
            {
                start_stopper_timer(timer_event_handler, STOPPER_MODE, 0);
            }
            else
            {
                NRF_LOG_INFO("Other...");
            }
        }
    }
}

#endif