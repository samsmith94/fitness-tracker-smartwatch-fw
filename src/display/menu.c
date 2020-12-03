


#include "menu.h"

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "nrf_error.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define BUTTON_1 NRF_GPIO_PIN_MAP(0, 13)

volatile uint32_t gesture_timer_cnt = 0;
volatile uint32_t button1_pressed_cnt = 0;

void button_1_handler(void)
{
    NRF_LOG_INFO("BUTTON_1 pressed, %d", gesture_timer_cnt);
    button1_pressed_cnt = gesture_timer_cnt;
}


void button_1_init(void)
{
	ret_code_t err_code;

	if (!nrf_drv_gpiote_is_init())
	{
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

	err_code = nrf_drv_gpiote_in_init(BUTTON_1, &in_config, button_1_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(BUTTON_1, true);
}


active_screen_t active_screen = MAIN_SCREEN;



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

/******************************************************************************/

static char battery_buff[5] = "";
static char step_count_buff[6] = "";

static char stopper_buff[8] = "";
static char stopper_buff2[8] = "";

static char timer_buff[8] = "";

/******************************************************************************/

void render_main_screen(void)
{
    active_screen = MAIN_SCREEN;

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


void render_stopper_screen(void)
{
    active_screen = STOPPER_SCREEN;

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

void render_timer_screen(void)
{
    active_screen = TIMER_SCREEN;

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
    active_screen = ACTIVITY_SCREEN;

    st7735_fill_screen(ST7735_BLACK);

    sprintf(stopper_buff, "%s", "Activity");
    ST7735_write_string(80 - 25, 26, stopper_buff, Font_11x18, ST7735_WHITE, ST7735_BLACK);

    ST7735_write_string(80 - 25, 46, "Double tap", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
    ST7735_write_string(80 - 25, 56, "to start", Font_7x10, ST7735_COLOR565(128, 128, 128), ST7735_BLACK);
}

