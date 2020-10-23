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

#define IMU_INT1 NRF_GPIO_PIN_MAP(0, 7)
#define IMU_INT2 NRF_GPIO_PIN_MAP(0, 8)

//#define LSM6DSOX_USE_SPI
#define LSM6DSOX_USE_I2C

#if defined(LSM6DSOX_USE_I2C)

#define IMU_TWI_SCL_PIN     NRF_GPIO_PIN_MAP(0, 5)
#define IMU_TWI_SDA_PIN     NRF_GPIO_PIN_MAP(0, 6)

#elif defined(LSM6DSOX_USE_SPI)

#define IMU_SPI_SCK_PIN NRF_GPIO_PIN_MAP(0, 29)
#define IMU_SPI_MISO_PIN NRF_GPIO_PIN_MAP(0, 29)
#define IMU_SPI_MOSI_PIN NRF_GPIO_PIN_MAP(0, 29)
#define IMU_SPI_SS_PIN NRF_GPIO_PIN_MAP(0, 29)

#endif



void lsm6dsox_double_tap_init(void);
void lsm6dsox_tilt_init(void);
void lsm6dsox_fifo_pedo_init(void);

extern uint16_t step_count;

void lsm6dsox_read_steps(void);

void lsm6dsox_read_data_init(void);
void lsm6dsox_fsm_init(void);

#endif /* INC_PLATFORM_H_ */
