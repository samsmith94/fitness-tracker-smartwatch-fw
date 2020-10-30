//#define BLE_CUS_TEST

#if defined(BLE_CUS_TEST)
#include "../ble/ble_cus.h"
#else

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#include "gesture/apds9960.h"
#include "display/st7735.h"
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

//#include "nrf_timer.h"
#endif

char RTT_String[20] = {0};

bool RTT_GetKey(void)
{
    int key;
    static int rx_index = 0;
    memset(RTT_String, '\0', 20);

    do
    {
        key = SEGGER_RTT_GetKey();
        if (key > 0)
        {
            RTT_String[rx_index] = (char)key;
            rx_index++;
        }
    } while ((key != '\n'));
    if (key == '\n')
    {
        //SEGGER_RTT_printf(0, "Received: %s", RTT_String);
        rx_index = 0;
        return true;
    }
    return false;
}

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
        if (RTT_GetKey()) {
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

static inline void LOG_BINARY_DUMP(uint8_t byte)
{
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'), (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'), (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'), (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("%s", bin);
}

/******************************************************************************/


/******************************************************************************/

typedef struct level
{
    struct level *next;
    struct level *prev;
    void (*render)(void);
} level;

void build_menu(struct level *current_node, struct level *prev_node, struct level *next_node, void (*render)(void))
{
    current_node->prev = prev_node;
    current_node->next = next_node;
    current_node->render = render;
}

void next(struct level **current_node)
{
    if ((*current_node)->next != 0)
    {
        (*current_node) = (*current_node)->next;
        st7735_fill_screen(ST7735_BLACK);
        (*current_node)->render();
    }
}

void prev(struct level **current_node)
{
    if ((*current_node)->prev != 0)
    {
        (*current_node) = (*current_node)->prev;
        st7735_fill_screen(ST7735_BLACK);
        (*current_node)->render();
        ;
    }
}

char battery_buff[5] = "";
char step_count_buff[6] = "";

void render_main_screen(void)
{
    draw_widget(bluetooth_widget, 10, 40 - (14 / 2));

    _time = nrf_cal_get_time();
    sprintf(time_buff, "%02d:%02d", _time->tm_hour, _time->tm_min);
    sprintf(date_buff, "%d/%d %s", _time->tm_mon + 1, _time->tm_mday, week_days[_time->tm_wday]);

    ST7735_write_string(80 - 25, 26, time_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);
    ST7735_write_string(80 - 25, 46, date_buff, Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);

    draw_widget(battery_widget, 135, 5);
    //sprintf(battery_buff, "%d%%", 100);
    sprintf(battery_buff, "%d", 100);
    ST7735_write_string(130, 20, battery_buff, Font_7x10, ST7735_MAGENTA, ST7735_BLACK);

    draw_widget(steps_widget, 135, 42);
    sprintf(step_count_buff, "%d", 5231);
    ST7735_write_string(130, 63, step_count_buff, Font_7x10, ST7735_YELLOW, ST7735_BLACK);
}

char stopper_buff[8] = "";
char stopper_buff2[8] = "";

void render_stopper_screen(void)
{
    st7735_fill_screen(ST7735_BLACK);
    //draw_widget(clock_widget, 10, 40 - (14 / 2));
    draw_widget(clk_widget, 10, 40 - (14 / 2));

    sprintf(stopper_buff, "%02d:%02d.", 0, 0, 0);
    ST7735_write_string(80 - 25, 26, stopper_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    sprintf(stopper_buff2, "%01d", 0);
    ST7735_write_string(80 - 25 + 65, 33, stopper_buff2, Font_7x10, ST7735_WHITE, ST7735_BLACK);

    ST7735_write_string(80 - 25, 46, "Double tap", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
    ST7735_write_string(80 - 25, 56, "to start", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
}

char timer_buff[8] = "";

void render_timer_screen(void)
{
    st7735_fill_screen(ST7735_BLACK);
    draw_widget(tim_widget, 10, 40 - (14 / 2));

    sprintf(stopper_buff, "%02d", 10);
    ST7735_write_string(80 - 25, 26, stopper_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    sprintf(stopper_buff2, "%01d", 0);
    ST7735_write_string(80 - 25 + 20, 33, " min", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    draw_widget(arrow_widget, 110, 40 - (14 / 2) - 8);

    ST7735_write_string(80 - 25, 46, "Swipe to set", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
}

void render_activity_screen(void)
{
    st7735_fill_screen(ST7735_BLACK);

    sprintf(stopper_buff, "%s", "Activity");
    ST7735_write_string(80 - 25, 26, stopper_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    ST7735_write_string(80 - 25, 46, "Double tap", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
    ST7735_write_string(80 - 25, 56, "to start", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
}

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
typedef struct
{
    uint8_t min;
    uint8_t sec;
    uint8_t tenth_of_sec;
} stopper_counter_t;

stopper_counter_t stopper;
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

        /*
			tenth_of_seconds--;
			timer.min = tenth_of_seconds / (60 * 10);
			timer.sec = (tenth_of_seconds - (timer.min * 60 * 10)) / 10;
			timer.tenth_of_sec = tenth_of_seconds - (timer.min * 60 * 10) - (timer.sec * 10);
			NRF_LOG_INFO("Timer: %02d:%02d.%01d\r\n", timer.min, timer.sec, timer.tenth_of_sec);
			if (tenth_of_seconds == 0) {
				nrf_drv_timer_disable(&TIMER_TEST);
			}
            */

        /*
			timer.tenth_of_sec++;
			if (timer.tenth_of_sec == 10) {
				timer.tenth_of_sec = 0;
				timer.sec++;
				if (timer.sec == 59) {
					timer.sec = 0;
					timer.min++;
				}
			}
			NRF_LOG_INFO("Stopper: %02d:%02d.%02d\r\n", timer.min, timer.sec, timer.tenth_of_sec);
			*/
        break;

    default:
        //Do nothing.
        break;
    }

    if (timer_event_handler_cnt == expiry_time)
    {
        timer_expired = true;
        nrf_drv_timer_disable(&TIMER_TEST);
        NRF_LOG_INFO("Timer expired.");
        st7735_sleep_in();
        //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, 0));
        set_display_pwm(0);
        //TODO: háttérvilágítás lekapcsolása

        //i = 0;

        timer_event_handler_cnt = 0;
        //ENTER_SYSTEM_ON_SLEEP_MODE();
        //nrf_pwr_mgmt_run();
    }
}

void start_keepalive_timer(nrfx_timer_event_handler_t timer_event_cb)
{
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

    //NRF_LOG_INFO("Hello World");

    //http://patorjk.com/software/taag/#p=display&f=Doom&t=Samu%20Sung%20Band

    //Doom

    //http://www.network-science.de/ascii/
    //smamslant:
    char welcome[] = "\n\
   ____                  ____                 ___               __\n\
  / __/__ ___ _  __ __  / __/_ _____  ___ _  / _ )___ ____  ___/ /\n\
 _\\ \\/ _ `/  ' \\/ // / _\\ \\/ // / _ \\/ _ `/ / _  / _ `/ _ \\/ _  / \n\
/___/\\_,_/_/_/_/\\_,_/ /___/\\_,_/_//_/\\_, / /____/\\_,_/_//_/\\_,_/  \n\
                                    /___/                         ";
    NRF_LOG_INFO("%s", welcome);

    nrf_delay_ms(20);
    st7735_init();
    nrf_delay_ms(20);
    st7735_fill_screen(ST7735_BLACK);

    //st7735_invert_colors(false);

    draw_widget(heart_widget, 10, 40 - (14 / 2) - 25);
    nrf_delay_ms(1000);
    draw_widget(facebook_widget, 30, 40 - (14 / 2));
    nrf_delay_ms(1000);

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

    lsm6dsox_tap_init();
    //lsm6dsox_read_data_init();

    lsm6dsox_fifo_pedo_init();

    /* Display ****************************************************************/
    init_display_pwm();
    st7735_sleep_out();

    /* Buzzer *****************************************************************/
    buzzer_init();
    test_rtttl_player();

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
        
        gesture_received = apds9960_read_gesture();
        apds9960_gesture_to_uart(gesture_received);

        if (gesture_received == APDS9960_RIGHT)
        {
            prev(&current_menu);
            st7735_sleep_out();
            //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, 5));
            set_display_pwm(5);
            timer_event_handler_cnt = 0;
            start_keepalive_timer(timer_event_handler);
        }
        else if (gesture_received == APDS9960_LEFT)
        {
            next(&current_menu);
            st7735_sleep_out();
            //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, 5));
            set_display_pwm(5);
            timer_event_handler_cnt = 0;
            start_keepalive_timer(timer_event_handler);
        }
        else if (gesture_received == ADPS9960_INVALID)
        {
            lsm6dsox_read_steps();
            NRF_LOG_INFO("WHILE");
        }
        nrf_delay_ms(50);
        

        /**********************************************************************/

        /*
        //ez egy kicsit szétcseszi a gesture-t....
        if (RTT_GetKey()) {
            NRF_LOG_INFO("Received from RTT Viewer: %s", RTT_String);
        }
        */
    }
}

#endif