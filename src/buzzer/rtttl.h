
#ifndef RTTTL_H
#define RTTTL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_gpio.h"

#define PIN_OUT NRF_GPIO_PIN_MAP(0, 11)

void play_note(float duration, float frequency);
void buzzer_init(void);
void test_rtttl_player(void);

#endif