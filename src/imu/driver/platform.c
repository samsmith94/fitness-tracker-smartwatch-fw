/*
 ******************************************************************************
 * @file    single_double_tap.c
 * @author  Sensors Software Solution Team
 * @brief   This file shows how to detect single and double tap from sensor.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/*
 * This example was developed using the following STMicroelectronics
 * evaluation boards:
 *
 * - STEVAL_MKI109V3 + STEVAL-MKI197V1
 * - NUCLEO_F411RE + STEVAL-MKI197V1
 *
 * and STM32CubeMX tool with STM32CubeF4 MCU Package
 *
 * Used interfaces:
 *
 * STEVAL_MKI109V3    - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * NUCLEO_STM32F411RE - Host side: UART(COM) to USB bridge
 *                    - I2C(Default) / SPI(supported)
 *
 * If you need to run this example on a different hardware platform a
 * modification of the functions: `platform_write`, `platform_read`,
 * `tx_com` and 'platform_init' is required.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>

#include "lsm6dsox_reg.h"
#include "platform.h"

stmdev_ctx_t g_dev_ctx;

/*
 * Gesture	TWI0
 * IMU		TWI1 / SPI1		sdk_config.h !!!
 * DISPLAY	SPI2
 */

#if defined(LSM6DSOX_USE_I2C)

const nrf_drv_twi_t imu_m_twi = NRF_DRV_TWI_INSTANCE(1);

const nrf_drv_twi_config_t imu_twi_config = {
	.scl = IMU_TWI_SCL_PIN,
	.sda = IMU_TWI_SDA_PIN,
	.frequency = NRF_DRV_TWI_FREQ_100K,
	.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
	.clear_bus_init = false};

static uint8_t imu_i2c_write_buf[2];

static void imu_i2c_init(nrf_drv_twi_t m_twi, nrf_drv_twi_config_t twi_config)
{
	ret_code_t err_code;

	err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
	APP_ERROR_CHECK(err_code);

	nrf_drv_twi_enable(&m_twi);
}

static void imu_i2c_read(nrf_drv_twi_t *m_twi, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
	ret_code_t ret;

	ret = nrf_drv_twi_tx(m_twi, slave_addr, (uint8_t *)&reg_addr, 1, false);
	if (NRF_SUCCESS != ret)
	{
		return;
	}

	ret = nrf_drv_twi_rx(m_twi, slave_addr, buff, size);

	APP_ERROR_CHECK(ret);
}

static void imu_i2c_write(nrf_drv_twi_t *m_twi, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
	ret_code_t ret;

	imu_i2c_write_buf[0] = reg_addr;
	imu_i2c_write_buf[1] = buff[0];
	ret = nrf_drv_twi_tx(m_twi, slave_addr, imu_i2c_write_buf, 2, false);
	APP_ERROR_CHECK(ret);
}

#elif defined(LSM6DSOX_USE_SPI)

#define SPI_INSTANCE 1
static const nrf_drv_spi_t imu_spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

nrf_drv_spi_config_t imu_spi_config = {
	.sck_pin = IMU_SPI_SCK_PIN,
	.mosi_pin = IMU_SPI_MOSI_PIN,
	.miso_pin = IMU_SPI_MISO_PIN,
	.ss_pin = IMU_SPI_SS_PIN,
	.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
	.orc = 0xFF,
	.frequency = NRF_DRV_SPI_FREQ_4M,
	.mode = NRF_DRV_SPI_MODE_0,
	.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST};

static void imu_spi_init(nrf_drv_spi_t spi, nrf_drv_spi_config_t spi_config)
{
	ret_code_t err_code;

	err_code = nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
	return err_code;
}

static void imu_spi_read(nrf_drv_spi_t *spi, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
	APP_ERROR_CHECK(nrf_drv_spi_transfer(spi, &reg_addr, 1, buff, size));
}

static void imu_spi_write(nrf_drv_spi_t *spi, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
	APP_ERROR_CHECK(nrf_drv_spi_transfer(spi, &reg_addr, 1, NULL, 0));
	APP_ERROR_CHECK(nrf_drv_spi_transfer(spi, buff, size, NULL, 0));
}
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint8_t whoamI, rst;
uint8_t tx_buffer[1000];

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void int2_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void int2_pin_init(void);

static void int1_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void int1_pin_init(void);

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
#if defined(LSM6DSOX_USE_I2C)
	imu_i2c_write(handle, (uint8_t)LSM6DSOX_I2C_ADD_L >> 1, reg, bufp, len);
#elif defined(LSM6DSOX_USE_SPI)
	imu_spi_write(handle, reg, bufp, len);
#endif
	return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
#if defined(LSM6DSOX_USE_I2C)
	imu_i2c_read(handle, (uint8_t)LSM6DSOX_I2C_ADD_L >> 1, reg, bufp, len);
#elif defined(LSM6DSOX_USE_SPI)
	/* Read command */
	reg |= 0x80;
	imu_spi_write(handle, reg, bufp, len);
#endif
	return 0;
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  tx_buffer     buffer to trasmit
 * @param  len           number of byte to send
 *
 */
void tx_com(uint8_t *tx_buffer, uint16_t len)
{
	NRF_LOG_INFO("%s", tx_buffer);
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
void platform_delay(uint32_t ms)
{
	nrf_delay_ms(ms);
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
void platform_init(void)
{
	ret_code_t err_code;

	int1_pin_init();
	int2_pin_init();

#if defined(LSM6DSOX_USE_I2C)
	imu_i2c_init(imu_m_twi, imu_twi_config);
#elif defined(LSM6DSOX_USE_SPI)
	imu_spi_init(imu_spi, imu_spi_config);
#endif
}

static void int2_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_INFO("int2_pin_handler() called.");
	
	//lsm6dsox_double_tap_irq_handler();

	


	//lsm6dsox_read_data_init_irq_handler();
	//lsm6dsox_fifo_pedo_irq_handler();
}

static void int2_pin_init(void)
{
	ret_code_t err_code;

	if (!nrf_drv_gpiote_is_init())
	{
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

	err_code = nrf_drv_gpiote_in_init(IMU_INT2, &in_config, int2_pin_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(IMU_INT2, true);
}

static void int1_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	NRF_LOG_INFO("int1_pin_handler() called.");

	fsm_multiconf_iqr_handler();
	//lsm6dsox_fsm_irq_handler();
	//lsm6dsox_multi_conf_irq_handler();
}

static void int1_pin_init(void)
{
	ret_code_t err_code;

	if (!nrf_drv_gpiote_is_init())
	{
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

	err_code = nrf_drv_gpiote_in_init(IMU_INT1, &in_config, int1_pin_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(IMU_INT1, true);
}
