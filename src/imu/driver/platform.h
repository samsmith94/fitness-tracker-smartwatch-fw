/*
 * platform.h
 *
 *  Created on: Aug 3, 2020
 *      Author: sunik
 */

#ifndef INC_PLATFORM_H_
#define INC_PLATFORM_H_

#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"

#include "nrf_drv_twi.h"

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrfx_spim.h"

void example_main_double_tap_lsm6dsox(void);

void example_main_tilt_lsm6dsox(void);
void lsm6dsox_activity(void);
void lsm6dsox_fifo_pedo_simple(void);
void example_main_orientation_lsm6dsox(void);
void lsm6dsox_read_data_simple(void);
void lsm6dsox_fsm(void);

#endif /* INC_PLATFORM_H_ */
