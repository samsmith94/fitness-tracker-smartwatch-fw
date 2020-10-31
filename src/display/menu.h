#ifndef MENU_H
#define MENU_H

#include "st7735.h"
#include "../rtc/calendar.h"

#include "nrf.h"
#include "nrf_drv_gpiote.h"

typedef enum {
    MAIN_SCREEN,
    STOPPER_SCREEN,
    TIMER_SCREEN,
    ACTIVITY_SCREEN
} active_screen_t;

active_screen_t active_screen;

typedef struct level
{
    struct level *next;
    struct level *prev;
    void (*render)(void);
} level;

void build_menu(struct level *current_node, struct level *prev_node, struct level *next_node, void (*render)(void));
void next(struct level **current_node);
void prev(struct level **current_node);
void render_main_screen(void);
void render_stopper_screen(void);
void render_timer_screen(void);
void render_activity_screen(void);



void button_1_init(void);
void button_1_handler(void);



#endif