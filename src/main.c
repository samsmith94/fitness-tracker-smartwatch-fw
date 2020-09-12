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

//#define TEST_BLE
#ifdef TEST_BLE

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

//#include "gesture/apds9960_common.h"
//#include "gesture/apds9960_proximity.h"
#include "gesture/apds9960.h"


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


#define BYTE_TO_BINARY(byte)    \
  (byte & 0x80 ? '1' : '0'),    \
  (byte & 0x40 ? '1' : '0'),    \
  (byte & 0x20 ? '1' : '0'),    \
  (byte & 0x10 ? '1' : '0'),    \
  (byte & 0x08 ? '1' : '0'),    \
  (byte & 0x04 ? '1' : '0'),    \
  (byte & 0x02 ? '1' : '0'),    \
  (byte & 0x01 ? '1' : '0')

//NRF_LOG_INFO("7|6|5|4|3|2|1|0");

static inline void LOG_BINARY_DUMP(uint8_t byte)
{
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'), (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'), (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'), (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("%s", bin);
}

static bool run_time_updates = false;

void print_current_time()
{
    NRF_LOG_INFO("Uncalibrated time:\t%s\r\n", nrf_cal_get_time_string(false));
    NRF_LOG_INFO("Calibrated time:\t%s\r\n", nrf_cal_get_time_string(true));
}


void calendar_updated()
{
    if(run_time_updates)
    {
        print_current_time();
    }
}

/******************************************************************************/


void set_date_and_time(void)
{
	struct tm tm = { 0 };

	char time_and_date_str[21] = { 0 };
	strncpy(time_and_date_str, __DATE__, strlen(__DATE__));
	time_and_date_str[strlen(__DATE__)] = ' ';
	strncpy(&time_and_date_str[strlen(__DATE__) + 1], __TIME__, strlen(__TIME__));

	char *s = strptime(time_and_date_str, "%b %d %Y %H:%M:%S", &tm);
	if (s == NULL) {
		NRF_LOG_INFO("Cannot parse date.\n");
	}

    nrf_cal_set_time(tm.tm_year+2000, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}
/******************************************************************************/

typedef enum {
	ADPS9960_INVALID, APDS9960_UP, APDS9960_DOWN, APDS9960_LEFT, APDS9960_RIGHT
} apds9960_gesture_received_t;

uint8_t gest_cnt = 0;
uint8_t u_count = 0;
uint8_t d_count = 0;
uint8_t l_count = 0;
uint8_t r_count = 0;


void gesture_init(void)
{
    uint8_t temp = 252;

    //atime
    i2c_write(APDS9960_ADDR, 0x81, &temp, 1);

    //again
	i2c_read(APDS9960_ADDR, 0x8F, &temp, 1);
	temp |= (1 << 0);
	i2c_write(APDS9960_ADDR, 0x8F, &temp, 1);

	//gconf4
	i2c_read(APDS9960_ADDR, 0xAB, &temp, 1);
	temp &= ~(1 << 0);
	i2c_write(APDS9960_ADDR, 0xAB, &temp, 1);

	//enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp &= ~(1 << 6);
	temp &= ~(1 << 2);
	temp &= ~(1 << 1);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);

	//enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp &= ~(1 << 5);
	temp &= ~(1 << 4);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);

	//aiclear
	temp = 0;
	i2c_write(APDS9960_ADDR, 0xE7, &temp, 1);

	//disable-enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp &= ~(1 << 0);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);
	nrf_delay_ms(10);
	temp |= (1 << 0);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);
    nrf_delay_ms(10);

	//gconf3 gdims
	temp = 0;
	//i2c_write(APDS9960_ADDR, 0xAA, &temp, 1);
    //csak bal-jobb:
    //temp = 0b00000010;
    //csak fel-le
    temp= 0b00000001;
    i2c_write(APDS9960_ADDR, 0xAA, &temp, 1);

	//gconf1 gfifith
	i2c_read(APDS9960_ADDR, 0xA2, &temp, 1);
	temp |= 0b01000000;
	i2c_write(APDS9960_ADDR, 0xA2, &temp, 1);

	//gconf2 ggain
	i2c_read(APDS9960_ADDR, 0xA3, &temp, 1);
	temp |= 0b01100000;
	i2c_write(APDS9960_ADDR, 0xA3, &temp, 1);

	//gpenth
	temp = 50;
	i2c_write(APDS9960_ADDR, 0xA0, &temp, 1);

	//gplulse
	temp = 0b11001001;
	i2c_write(APDS9960_ADDR, 0xA6, &temp, 1);

	//enable
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp |= (1 << 6);
	temp |= (1 << 2);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);

	uint8_t id;
	i2c_read(APDS9960_ADDR, 0x92, &id, 1);
	NRF_LOG_INFO("gesture initialized, %d", id);
}

void apds9960_reset_counts(void)
{
	gest_cnt = 0;
	u_count = 0;
	d_count = 0;
	l_count = 0;
	r_count = 0;
}

void apds9960_gesture_to_uart(apds9960_gesture_received_t gesture_received)
{
	switch (gesture_received) {
	case APDS9960_UP:
		NRF_LOG_INFO("UP");
		break;
	case APDS9960_DOWN:
		NRF_LOG_INFO("DOWN");
		break;
	case APDS9960_LEFT:
		NRF_LOG_INFO("LEFT");
		break;
	case APDS9960_RIGHT:
		NRF_LOG_INFO("RIGHT");
		break;
	default:
		break;
	}
}

apds9960_gesture_received_t apds9960_read_gesture(void)
{
	//printf("read gesture function %d\r\n", HAL_GetTick());
	uint8_t to_read;
	uint8_t buf[256];
	unsigned long t = 0;
	apds9960_gesture_received_t gesture_received;

	while (1) {
		int up_down_diff = 0;
		int left_right_diff = 0;
		gesture_received = 0;

		/* is gesture valid */
		uint8_t temp;
		i2c_read(APDS9960_ADDR, 0xAF, &temp, 1);

		if (!(temp & (1 << 0))) {
			return ADPS9960_INVALID;
		}

		nrf_delay_ms(30);

		/* getting the nember of samples to be read */
		temp = 0;
		i2c_read(APDS9960_ADDR, 0xAE, &temp, 1);
		//printf("Samples to read: %d\r\n", temp);

		to_read = temp;

		// bytesRead is unused but produces sideffects needed for readGesture to work

		//APDS9960_GFIFO_U
		i2c_read(APDS9960_ADDR, 0xFC, buf, to_read);

		if (abs((int) buf[0] - (int) buf[1]) > 13)
			up_down_diff += (int) buf[0] - (int) buf[1];

		if (abs((int) buf[2] - (int) buf[3]) > 13)
			left_right_diff += (int) buf[2] - (int) buf[3];

		if (up_down_diff != 0) {
			if (up_down_diff < 0) {
				if (d_count > 0) {
					gesture_received = APDS9960_UP;
				} else
					u_count++;
			} else if (up_down_diff > 0) {
				if (u_count > 0) {
					gesture_received = APDS9960_DOWN;
				} else
					d_count++;
			}
		}

		if (left_right_diff != 0) {
			if (left_right_diff < 0) {
				if (r_count > 0) {
					gesture_received = APDS9960_LEFT;
				} else
					l_count++;
			} else if (left_right_diff > 0) {
				if (l_count > 0) {
					gesture_received = APDS9960_RIGHT;
				} else
					r_count++;
			}
		}

		if (up_down_diff != 0 || left_right_diff != 0)
			//t = HAL_GetTick();
            t = NRF_RTC0->COUNTER / 8;

		//if (gesture_received || HAL_GetTick() - t > 300) {
        if (gesture_received || (NRF_RTC0->COUNTER / 8) - t > 300) {
			apds9960_reset_counts();
			return gesture_received;
		}
	}
}



//LECSWERÉLVE A FENTI REGISZTES KÓDOKAT RENDES NEVESÍTETT FÜGGVÉYNKERE:

void new_gesture_init(void)
{
    //a kövi rész az adafruittól van:
    /**************************************************************************/
    //atime
    //set_als_adc_integration_time(252);        NINCS MÉG MEGVALÓSÍTVA

    //again
    /*
	i2c_read(APDS9960_ADDR, 0x8F, &temp, 1);
	temp |= (1 << 0);
	i2c_write(APDS9960_ADDR, 0x8F, &temp, 1);
    */

	//gconf4
    /*
	i2c_read(APDS9960_ADDR, 0xAB, &temp, 1);
	temp &= ~(1 << 0);
	i2c_write(APDS9960_ADDR, 0xAB, &temp, 1);
    */

	set_gesture_enabled(false);
    set_proximity_enabled(false);
    set_als_enabled(false);

    set_proximity_interrupt_enabled(false);
    set_als_interrupt_enabled(false);

    clear_all_non_gesture_interrupt();

	//disable-enable
    set_power_on(false);
	nrf_delay_ms(10);
	set_power_on(true);
    nrf_delay_ms(10);

	//gconf3 gdims
    apds9960_gdims_t gdims = APDS9960_BOTH_PAIRS_ACTICE;
    //apds9960_gdims_t gdims = APDS9960_UP_DOWN_ACTICE;
    //apds9960_gdims_t gdims = APDS9960_LEFT_RIGHT_ACTICE;
    set_gesture_dimension_select(gdims);

	//gconf1 gfifith
    apds9960_gfifoth_t gfifoth = APDS9960_INTERRUPT_AFTER_4_DATASET_ADDED_TO_FIFO;
    set_gesture_fifo_threshold(gfifoth);

	//gconf2 ggain
    apds9960_ggain_t ggain = APDS9960_G_GAIN_4X;
    set_gesture_gain(ggain);


	//gpenth
    set_gesture_proximity_enter_threshold(50);

	//gplulse
    apds9960_gplen_t gplen = APDS9960_G_PULSE_LENGTH_32_US;
    apds9960_set_gesture_pulse_length(gplen);

    //ez valójában 10-et jelent! nem kéne átírni a függvéynt hogy ő adjon hozzá?
    set_gesture_pulse_count(9);
    
    //eddig tart az adarfruit
    /**************************************************************************/

    //ez nem tudom mit keres itt, hiszen ez a gplen és a pulsecount
    //csak itt 16_US ezzel a beállítással:
    /*
	temp = 0b11001001;
	i2c_write(APDS9960_ADDR, 0xA6, &temp, 1);
    */

	//enable
    /*
	i2c_read(APDS9960_ADDR, 0x80, &temp, 1);
	temp |= (1 << 6);
	temp |= (1 << 2);
	i2c_write(APDS9960_ADDR, 0x80, &temp, 1);
    */
    // a fenti megfelelője:
    set_proximity_enabled(true);
    set_gesture_enabled(true);

    //ez az id vizsgálat lehetne a legelején is, sőt ott célszerűbb volna talán
    /*
	uint8_t id;
	i2c_read(APDS9960_ADDR, 0x92, &id, 1);
	NRF_LOG_INFO("gesture initialized, %d", id);
    */
    uint8_t id = get_device_id();
    if (id == 0xAB) {
        NRF_LOG_INFO("Gesture initialized successfully.");
    } else {
        NRF_LOG_INFO("Gesture initialization failed.");
    }

   //MÉG NÉHÁNY INTERRUPTOT ÉS BE LEHETNE ÁLLÍTANI....
}



apds9960_gesture_received_t new_apds9960_read_gesture(void)
{
    //gesture_fifo_t gesture_fifo_buffer[32];

	uint8_t to_read;
	uint8_t buf[256];
	unsigned long t = 0;
	apds9960_gesture_received_t gesture_received;

	while (1) {
		int up_down_diff = 0;
		int left_right_diff = 0;
		gesture_received = 0;

		/* is gesture valid */
        //még nincs implementálva...., azt sem találtam ki hogy legyen a státuszlekérdezés
        /*
        if (!is_gesture_valid()) {
            return ADPS9960_INVALID;
        }
        */
		uint8_t temp;
		i2c_read(APDS9960_ADDR, 0xAF, &temp, 1);

		if (!(temp & (1 << 0))) {
			return ADPS9960_INVALID;
		}

		nrf_delay_ms(30);

		/* getting the number of samples to be read */
        to_read = get_gesture_fifo_level();

		// bytesRead is unused but produces sideffects needed for readGesture to work

		//APDS9960_GFIFO_U
		i2c_read(APDS9960_ADDR, 0xFC, buf, to_read);

		if (abs((int) buf[0] - (int) buf[1]) > 13)
			up_down_diff += (int) buf[0] - (int) buf[1];

		if (abs((int) buf[2] - (int) buf[3]) > 13)
			left_right_diff += (int) buf[2] - (int) buf[3];

		if (up_down_diff != 0) {
			if (up_down_diff < 0) {
				if (d_count > 0) {
					gesture_received = APDS9960_UP;
				} else
					u_count++;
			} else if (up_down_diff > 0) {
				if (u_count > 0) {
					gesture_received = APDS9960_DOWN;
				} else
					d_count++;
			}
		}

		if (left_right_diff != 0) {
			if (left_right_diff < 0) {
				if (r_count > 0) {
					gesture_received = APDS9960_LEFT;
				} else
					l_count++;
			} else if (left_right_diff > 0) {
				if (l_count > 0) {
					gesture_received = APDS9960_RIGHT;
				} else
					r_count++;
			}
		}

		if (up_down_diff != 0 || left_right_diff != 0)
			//t = HAL_GetTick();
            t = NRF_RTC0->COUNTER / 8;

		//if (gesture_received || HAL_GetTick() - t > 300) {
        if (gesture_received || (NRF_RTC0->COUNTER / 8) - t > 300) {
			apds9960_reset_counts();
			return gesture_received;
		}
	}
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



    //twi_init();
    

    nrf_delay_ms(20);
    st7735_init();
    nrf_delay_ms(20);
    st7735_fill_screen(ST7735_BLACK);
    
    //ST7735_BLUE
    //ST7735_write_string(0, 10, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.", Font_7x10, ST7735_WHITE, ST7735_BLUE);
    /*
    ST7735_write_string(80-25, 26, "22:04", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    ST7735_write_string(80-25, 46, "6/26 Fri", Font_7x10, ST7735_WHITE, ST7735_BLACK);
    */
   
    circle_t p_crcl;
    p_crcl.x = 80;
    p_crcl.y = 10;
    p_crcl.r = 5;
    //fill_circle_draw(&p_crcl, ST7735_MAGENTA);
    //ST7735_CYAN

    
    st7735_fill_rectangle(80+45, 10, 4, 8, ST7735_GREEN);
    st7735_fill_rectangle(85+45, 10, 4, 8, ST7735_GREEN);
    st7735_fill_rectangle(90+45, 10, 4, 8, ST7735_GREEN);
    st7735_fill_rectangle(95+45, 10, 4, 8, ST7735_GREEN);

    line_draw(79+45, 9, 99+45, 9, ST7735_COLOR565(128, 128, 128));
    line_draw(79+45, 18, 99+45, 18, ST7735_COLOR565(128, 128, 128));

    line_draw(79+45, 9, 79+45, 18, ST7735_COLOR565(128, 128, 128));
    line_draw(99+45, 9, 99+45, 18, ST7735_COLOR565(128, 128, 128));

    st7735_fill_rectangle(99+45, 9+3, 4, 4, ST7735_COLOR565(128, 128, 128));

    ST7735_write_string(125, 25, "78%", Font_7x10, ST7735_COLOR565(0, 128, 0), ST7735_BLACK);

    /*
    circle_t p_circle;
    p_circle.x = 20;
    p_circle.y = 40;
    p_circle.r = 15;
    fill_circle_draw(&p_circle, ST7735_COLOR565(0,0,139));
    
    //x0, y0, y1, y1
    line_draw(20, 32, 20, 48, ST7735_WHITE);


    line_draw(20, 32, 24, 36, ST7735_WHITE);
    line_draw(24, 36, 16, 44, ST7735_WHITE);


    line_draw(20, 48, 24, 44, ST7735_WHITE);
    line_draw(24, 44, 16, 36, ST7735_WHITE);
    */

    
    draw_widget(bluetooth_widget, 10, 40-(14/2)-25);
    draw_widget(battery_widget, 10, 40-(14/2));
    draw_widget(clock_widget, 10, 40-(14/2)+25);
    
    //draw_widget(steps_widget, 30, 40-(14/2));
    
    draw_widget(facebook_widget, 30, 40-(14/2));
    //draw_widget(gmail_widget, 30, 40-(14/2));
    

    
    
    
    

    //p_circle.r = 20;
    //draw_circle(p_circle.x, p_circle.y, p_circle.r, ST7735_WHITE);


    //st7735_invert_colors(false);



    i2c_init();
    set_power_on(true);

    nrf_delay_ms(15);
    
    //is_power_on();

    set_proximity_enabled(true);
    //is_proximity_enabled();

    //get_device_id();
    
    /*
    set_proximity_interrupt_enabled(true);
    is_proximity_interrupt_enabled();



    set_proximity_interrupt_low_threshold(1+4+16+64);
    get_proximity_interrupt_low_threshold();
    */

    apds9960_ldrive_t ldrive = APDS9960_GLED_CURRENT_12_5_MA;
    set_proximity_led_drive_strength(ldrive);

    apds9960_pplen_t pulse_length = APDS9960_PULSE_LENGTH_16_US;
    set_proximity_pulse_length(pulse_length);
     get_proximity_pulse_length();

    set_proximity_pulse_count(1+4+16);
    get_proximity_pulse_count();

    apds9960_pgain_t pgain = APDS9960_P_GAIN_2X;
    set_proximity_gain(pgain);
    pgain = get_proximity_gain();

    NRF_LOG_INFO("START");

    /**************************************************************************/
    uint32_t year, month, day, hour, minute, second;

    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    nrf_cal_init();
    nrf_cal_set_callback(calendar_updated, 4);

    //nrf_cal_set_time(2020, 8, 9, 12, 35, 0);
    set_date_and_time();
    /**************************************************************************/

    apds9960_gesture_received_t gesture_received;
    gesture_init();
    
    int key;

    struct tm *time;
    char time_buff[50] = "";
    char date_buff[50] = "";
    char *week_days[] = {"Mon, Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    //time->tm_wday

    while (true)
    {
        /*
    NRF_LOG_INFO("*******************");
    set_gesture_offset_up(-127);
    int8_t valami = get_gesture_offset_up();
    NRF_LOG_INFO("%d", valami);*/


        /*
        gesture_received = apds9960_read_gesture();
        apds9960_gesture_to_uart(gesture_received);
        */

        print_current_time();
        nrf_delay_ms(1000);

        
        time = nrf_cal_get_time();
        sprintf(time_buff, "%02d:%02d", time->tm_hour, time->tm_min);
        sprintf(date_buff, "%d/%d %s", time->tm_mon + 1, time->tm_mday, week_days[time->tm_wday]);

        ST7735_write_string(80-25, 26, time_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);
        ST7735_write_string(80-25, 46, date_buff, Font_7x10, ST7735_WHITE, ST7735_BLACK);
        

       /*
        sprintf(time_buff, "%02d:%02d", 8, 30);
        sprintf(date_buff, "%d/%d %s", 8, 27, "Thu");
        ST7735_write_string(80-25, 26, time_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);
        ST7735_write_string(80-25, 46, date_buff, Font_7x10, ST7735_WHITE, ST7735_BLACK);
*/
        


        key = SEGGER_RTT_GetKey();
        if (key > 0) {
            NRF_LOG_INFO("Key: %c", key);
        }

        uint8_t val = 0b10000001;
        //LOG_BINARY_DUMP(128-val);
        //LOG_BINARY_DUMP(128-val);
        NRF_LOG_INFO("%d", val);
        NRF_LOG_INFO("** %d", 128-val);
        /*
        printf("\r\nUART example started.\r\n");
        //read_sensor_id();
        //LOG_BINARY_DUMP(171);        
        
        uint8_t prox = get_proximity_data();
        NRF_LOG_INFO("Proximity: %d", prox);
        nrf_delay_ms(1000);
        print_current_time();
        */

        /*
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
        }*/
    }
}

#endif