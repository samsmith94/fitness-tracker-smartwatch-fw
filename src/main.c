/**
 * Copyright (c) 2014 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup uart_example_main main.c
 * @{
 * @ingroup uart_example
 * @brief UART Example Application main file.
 *
 * This file contains the source code for a sample application using UART.
 *
 */

#define BLE_CUS_TEST
//#define TEST_BLE_UART
#ifdef TEST_BLE_UART

#include "../ble/ble_uart.h"

int main(void)
{
    bool erase_bonds;

    // Initialize.
    uart_init();
    log_init();
    timers_init();
    buttons_leds_init(&erase_bonds);
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    // Start execution.
    printf("\r\nUART started.\r\n");
    NRF_LOG_INFO("Debug logging for UART over RTT started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
#elif defined(BLE_CUS_TEST)

#include "../ble/ble_cus.h"

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

    int key;

    char RTT_String[20] = {0};
    int rx_index = 0;

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();

        /*
        if (rx_index == 0) {
            memset(RTT_String, '0', 20);
        }
        key = SEGGER_RTT_GetKey();
        if (key != '\n') {
            SEGGER_RTT_printf(0, "Received: %c.", key);
            RTT_String[rx_index++] = key;
        } else {
            rx_index = 0;
            NRF_LOG_INFO("%s", RTT_String);
            SEGGER_RTT_printf(0, "Received: %s.", RTT_String);
        }
        */

        do
        {
            key = SEGGER_RTT_GetKey();
            if (key > 0) {
                RTT_String[rx_index] = (char)key;
                rx_index++;
            }
            
        } while ((key != '\n'));
        if (key == '\n') {
            SEGGER_RTT_printf(0, "Received: %s", RTT_String);
            memset(RTT_String, '\0', 20);
            rx_index = 0;
            
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

//#include "gesture/apds9960.h"
#include "display/st7735.h"
//#include "display/fonts.h"

#include "gesture/apds9960.h"
#include "rtc/calendar.h"

#include "nrf_drv_timer.h"

#include "test_apds9960.h"

//#include "nrf_timer.h"

//#define ENABLE_LOOPBACK_TEST  /**< if defined, then this example will be a loopback test, which means that TX should be connected to RX to get data loopback. */

#define MAX_TEST_DATA_BYTES (15U) /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256      /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256      /**< UART RX buffer size. */

void uart_error_handle(app_uart_evt_t *p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED

static inline void LOG_BINARY_DUMP(uint8_t byte)
{
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'), (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'), (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'), (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("%s", bin);
}

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
    }
}

char battery_buff[5] = "";
char step_count_buff[6] = "";

void render_main_screen(void)
{
    //ST7735_write_string(80-25, 26, "1", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    //ST7735_write_string(80-25, 46, "MAIN", Font_7x10, ST7735_WHITE, ST7735_BLACK);

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
    //ST7735_write_string(80-25, 26, "2", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    //ST7735_write_string(80-25, 46, "STOPPER", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    st7735_fill_screen(ST7735_BLACK);
    draw_widget(clock_widget, 10, 40 - (14 / 2));

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
    //ST7735_write_string(80-25, 26, "3", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    //ST7735_write_string(80-25, 46, "TIMER", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    st7735_fill_screen(ST7735_BLACK);
    draw_widget(clock_widget, 10, 40 - (14 / 2));

    sprintf(stopper_buff, "%02d", 10);
    ST7735_write_string(80 - 25, 26, stopper_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    sprintf(stopper_buff2, "%01d", 0);
    ST7735_write_string(80 - 25 + 20, 33, " min", Font_7x10, ST7735_WHITE, ST7735_BLACK);

    draw_widget(arrow_widget, 110, 40 - (14 / 2) - 8);

    ST7735_write_string(80 - 25, 46, "Swipe to set", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
}

void render_activity_screen(void)
{
    //ST7735_write_string(80-25, 26, "4", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    //ST7735_write_string(80-25, 46, "ACTIVITY", Font_7x10, ST7735_WHITE, ST7735_BLACK);

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

/*
int main()
{
    //Configure RAM retention. More RAM retentpin_in_power_down_gpio_inition means increased current consumption (see electrical specification in the Product Specification, power chapter)
    configure_ram_retention();

    pin_in_power_down_gpio_init();

    while(1) {
        ENTER_SYSTEM_ON_SLEEP_MODE()
    }
}
*/
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

    //bsp_board_leds_on();

    /*
    const app_uart_comm_params_t comm_params =
    {
        NRF_GPIO_PIN_MAP(0, 29),
        NRF_GPIO_PIN_MAP(1, 13),
        NRF_GPIO_PIN_MAP(0, 31),
        NRF_GPIO_PIN_MAP(1, 12),
        UART_HWFC,
        false,
        NRF_UART_BAUDRATE_115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
    */

    /* Display ****************************************************************/

    NRF_LOG_INFO("Hello World");

    nrf_delay_ms(20);
    st7735_init();
    nrf_delay_ms(20);
    st7735_fill_screen(ST7735_BLACK);

    //draw_widget(heart_widget, 10, 40-(14/2)-25);
    //draw_widget(battery_widget, 140, 5);
    //draw_widget(clock_widget, 10, 40-(14/2)+25);
    //draw_widget(steps_widget, 30, 40-(14/2));
    //draw_widget(facebook_widget, 30, 40-(14/2));

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

    i2c_init();

    apds9960_gesture_received_t gesture_received;
    gesture_init();
    //new_gesture_init();
    gpio_init();

    /* Timer ******************************************************************/
    /*
    uint32_t time_ms = 100; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
	err_code = NRF_SUCCESS;

    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = NRF_TIMER_FREQ_62500Hz;
    err_code = nrf_drv_timer_init(&TIMER_TEST, &timer_cfg, timer_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_TEST, time_ms);

    nrf_drv_timer_extended_compare(&TIMER_TEST, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&TIMER_TEST);
    */

    /* RTT key ****************************************************************/
    int key;

    /* apds9960 test **********************************************************/

    //NRF_LOG_INFO("Hello world!");

    //test_ENABLE();
    //test_ATIME();
    //test_AILTL();

    /**************************************************************************/
    st7735_sleep_out();
    while (true)
    {
        /*
        st7735_sleep_in();
        nrf_delay_ms(1000);
        st7735_sleep_out();
        nrf_delay_ms(1000);
        */

        /*
        uint8_t status = get_device_status();
        if (status & (1 << APDS9960_PVALID)) {
            
            //kiolvasni a proximiti adatot...
            uint8_t proximity = get_proximity_data();

            NRF_LOG_INFO("Proximity: %d", proximity);
            //aztán handler: proximity_interrupt_event_handler(...);
            //clear_proximity_interrupt();
        }
        */

        /*
        nrf_delay_ms(100);
        NRF_LOG_INFO("Proximity: %d", get_proximity_data());
        */

        gesture_received = apds9960_read_gesture();
        apds9960_gesture_to_uart(gesture_received);

        if (gesture_received == APDS9960_RIGHT)
        {
            prev(&current_menu);
            st7735_sleep_out();
            timer_event_handler_cnt = 0;
            start_keepalive_timer(timer_event_handler);
        }
        else if (gesture_received == APDS9960_LEFT)
        {
            next(&current_menu);
            st7735_sleep_out();
            timer_event_handler_cnt = 0;
            start_keepalive_timer(timer_event_handler);
        }
        nrf_delay_ms(50);

        /**********************************************************************/
        /*
        _time = nrf_cal_get_time();
        sprintf(time_buff, "%02d:%02d", _time->tm_hour, _time->tm_min);
        sprintf(date_buff, "%d/%d %s", _time->tm_mon + 1, _time->tm_mday, week_days[_time->tm_wday]);
        ST7735_write_string(80-25, 26, time_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);
        ST7735_write_string(80-25, 46, date_buff, Font_7x10, ST7735_WHITE, ST7735_BLACK);
        //print_current_time();
        //nrf_delay_ms(1000);
        */

        /**********************************************************************/

        /*
        key = SEGGER_RTT_GetKey();
        if (key > 0) {
            NRF_LOG_INFO("Key: %c", key);
        }
        */

        /**********************************************************************/
        /*
        printf("\r\nUART example started.\r\n");
        uint8_t cr;
        while (app_uart_get(&cr) != NRF_SUCCESS);
        while (app_uart_put(cr) != NRF_SUCCESS);

        if (cr == 'q' || cr == 'Q')
        {
            printf(" \r\nExit!\r\n");

            while (true)
            {
                // Do nothing.
            }
        }
        */
    }
}

#endif