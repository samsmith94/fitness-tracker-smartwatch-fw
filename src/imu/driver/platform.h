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

typedef union
{
	int16_t i16bit[3];
	uint8_t u8bit[6];
} axis3bit16_t;

typedef union
{
	int16_t i16bit;
	uint8_t u8bit[2];
} axis1bit16_t;


typedef union
{
	struct
	{
		uint16_t step_count;
		uint32_t timestamp;
#ifdef __GNUC__
	} __attribute__((__packed__));
#else  /* __GNUC__ */
	};
#endif /* __GNUC__ */
	uint8_t byte[6];
} pedo_count_sample_t;

extern const nrf_drv_twi_t imu_m_twi;

extern stmdev_ctx_t g_dev_ctx;

extern uint16_t step_count;
extern uint8_t whoamI, rst;
extern uint8_t tx_buffer[1000];

int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
void tx_com(uint8_t *tx_buffer, uint16_t len);
void platform_delay(uint32_t ms);
void platform_init(void);

/******************************************************************************/

void lsm6dsox_tilt_init(void);
void lsm6dsox_fifo_pedo_init(void);

void lsm6dsox_read_steps(void);

void lsm6dsox_read_data_init(void);
void lsm6dsox_fsm_init(void);

void lsm6dsox_fifo_pedo_irq_handler(void);
void lsm6dsox_read_data_irq_handler(void);
void lsm6dsox_double_tap_irq_handler(void);

#endif /* INC_PLATFORM_H_ */
