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

//#define LSM6DSOX_USE_SPI
#define LSM6DSOX_USE_I2C

#ifdef LSM6DSOX_USE_I2C
#include "../driver/i2c.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(1);

//#define DEBUG_I2C

static uint8_t imu_i2c_write_buf[2];

void imu_i2c_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
        .scl = NRF_GPIO_PIN_MAP(0, 5),
        .sda = NRF_GPIO_PIN_MAP(0, 6),
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = false};

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    //err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

void imu_i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
    ret_code_t ret;

    ret = nrf_drv_twi_tx(&m_twi, slave_addr, (uint8_t *)&reg_addr, 1, false);
    if (NRF_SUCCESS != ret)
    {
        return;
    }

    //nrf_delay_ms(1);

    ret = nrf_drv_twi_rx(&m_twi, slave_addr, buff, size);

    APP_ERROR_CHECK(ret);

#ifdef DEBUG_I2C
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (buff[0] & 0x80 ? '1' : '0'), (buff[0] & 0x40 ? '1' : '0'), (buff[0] & 0x20 ? '1' : '0'), (buff[0] & 0x10 ? '1' : '0'), (buff[0] & 0x08 ? '1' : '0'), (buff[0] & 0x04 ? '1' : '0'), (buff[0] & 0x02 ? '1' : '0'), (buff[0] & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("I2C  READ(0x%02X): %s", reg_addr, bin);
    //NRF_LOG_INFO("****************************************");
#endif
}

void imu_i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size)
{
    ret_code_t ret;

    imu_i2c_write_buf[0] = reg_addr;
    imu_i2c_write_buf[1] = buff[0];
    ret = nrf_drv_twi_tx(&m_twi, slave_addr, imu_i2c_write_buf, 2, false);
    
/*
    ret = nrf_drv_twi_tx(&m_twi, slave_addr, (uint8_t *)&reg_addr, 1, true);
    if (NRF_SUCCESS != ret)
    {
        return;
    }
*/
    //nrf_delay_ms(1);

    //ret = nrf_drv_twi_tx(&m_twi, slave_addr, buff, size, false);
    APP_ERROR_CHECK(ret);

#ifdef DEBUG_I2C
    char bin[18];
    sprintf(bin, "[%c|%c|%c|%c|%c|%c|%c|%c]", (buff[0] & 0x80 ? '1' : '0'), (buff[0] & 0x40 ? '1' : '0'), (buff[0] & 0x20 ? '1' : '0'), (buff[0] & 0x10 ? '1' : '0'), (buff[0] & 0x08 ? '1' : '0'), (buff[0] & 0x04 ? '1' : '0'), (buff[0] & 0x02 ? '1' : '0'), (buff[0] & 0x01 ? '1' : '0'));
    NRF_LOG_INFO("I2C WRITE(0x%02X): %s", reg_addr, bin);
    //NRF_LOG_INFO("****************************************");
#endif
}
#else
//TODO: meg kell nézni hol lesznek a pinek, mert csak a kijelző pinjei lettek átmásolva
#define LSM6DSOX_INT_PIN NRF_GPIO_PIN_MAP(0, 29)
#define LSM6DSOX_SPI_SCK_PIN NRF_GPIO_PIN_MAP(0, 29)
#define LSM6DSOX_SPI_MISO_PIN NRF_GPIO_PIN_MAP(0, 29)
#define LSM6DSOX_SPI_MOSI_PIN NRF_GPIO_PIN_MAP(0, 29)
#define LSM6DSOX_SPI_SS_PIN NRF_GPIO_PIN_MAP(0, 29)

#define SPI_INSTANCE 1
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
#endif

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

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

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis1bit16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;

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

static uint8_t whoamI, rst;
static uint8_t tx_buffer[1000];

/* Program: glance */
static const uint8_t lsm6so_prg_glance[] = {
	0xb2,
	0x10,
	0x24,
	0x20,
	0x17,
	0x17,
	0x66,
	0x32,
	0x66,
	0x3c,
	0x20,
	0x20,
	0x02,
	0x02,
	0x08,
	0x08,
	0x00,
	0x04,
	0x0c,
	0x00,
	0xc7,
	0x66,
	0x33,
	0x73,
	0x77,
	0x64,
	0x88,
	0x75,
	0x99,
	0x66,
	0x33,
	0x53,
	0x44,
	0xf5,
	0x22,
	0x00,
};

/* Program: motion */
static const uint8_t lsm6so_prg_motion[] = {
	0x51,
	0x10,
	0x16,
	0x00,
	0x00,
	0x00,
	0x66,
	0x3c,
	0x02,
	0x00,
	0x00,
	0x7d,
	0x00,
	0xc7,
	0x05,
	0x99,
	0x33,
	0x53,
	0x44,
	0xf5,
	0x22,
	0x00,
};

/* Program: no_motion */
static const uint8_t lsm6so_prg_no_motion[] = {
	0x51,
	0x00,
	0x10,
	0x00,
	0x00,
	0x00,
	0x66,
	0x3c,
	0x02,
	0x00,
	0x00,
	0x7d,
	0xff,
	0x53,
	0x99,
	0x50,
};
/* Program: wakeup */
static const uint8_t lsm6so_prg_wakeup[] = {
	0xe2,
	0x00,
	0x1e,
	0x20,
	0x13,
	0x15,
	0x66,
	0x3e,
	0x66,
	0xbe,
	0xcd,
	0x3c,
	0xc0,
	0xc0,
	0x02,
	0x02,
	0x0b,
	0x10,
	0x05,
	0x66,
	0xcc,
	0x35,
	0x38,
	0x35,
	0x77,
	0xdd,
	0x03,
	0x54,
	0x22,
	0x00,
};

/* Program: pickup */
static const uint8_t lsm6so_prg_pickup[] = {
	0x51,
	0x00,
	0x10,
	0x00,
	0x00,
	0x00,
	0x33,
	0x3c,
	0x02,
	0x00,
	0x00,
	0x05,
	0x05,
	0x99,
	0x30,
	0x00,
};

/* Program: orientation */
static const uint8_t lsm6so_prg_orientation[] = {
	0x91,
	0x10,
	0x16,
	0x00,
	0x00,
	0x00,
	0x66,
	0x3a,
	0x66,
	0x32,
	0xf0,
	0x00,
	0x00,
	0x0d,
	0x00,
	0xc7,
	0x05,
	0x73,
	0x99,
	0x08,
	0xf5,
	0x22,
};

/* Program: wrist_tilt */
static const uint8_t lsm6so_prg_wrist_tilt[] = {
	0x52,
	0x00,
	0x14,
	0x00,
	0x00,
	0x00,
	0xae,
	0xb7,
	0x80,
	0x00,
	0x00,
	0x06,
	0x0f,
	0x05,
	0x73,
	0x33,
	0x07,
	0x54,
	0x44,
	0x22,
};

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void tx_com(uint8_t *tx_buffer, uint16_t len);
static void platform_delay(uint32_t ms);
static void platform_init(void);

#define LSM6DSOX_INT1 NRF_GPIO_PIN_MAP(0, 7)
#define LSM6DSOX_INT2 NRF_GPIO_PIN_MAP(0, 8)

static void int2_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

static void int2_pin_init(void);

stmdev_ctx_t g_dev_ctx;
/* Main Example --------------------------------------------------------------*/
void example_main_double_tap_lsm6dsox_init(void)
{
	/* Uncomment to configure INT 1 */
	//lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment to configure INT 2 */
	lsm6dsox_pin_int2_route_t int2_route;

	/* Initialize mems driver interface */
	g_dev_ctx.write_reg = platform_write;
	g_dev_ctx.read_reg = platform_read;
	//dev_ctx.handle = &hi2c1;

	/* Init test platform */
	platform_init();

	/* Wait sensor boot time */
	platform_delay(10);

	/* Check device ID */

	lsm6dsox_device_id_get(&g_dev_ctx, &whoamI);
	platform_delay(10);
	NRF_LOG_INFO("%d", whoamI);
	if (whoamI != LSM6DSOX_ID)
		while (1)
			;

	/* Restore default configuration */
	lsm6dsox_reset_set(&g_dev_ctx, PROPERTY_ENABLE);
	do
	{
		lsm6dsox_reset_get(&g_dev_ctx, &rst);
	} while (rst);

	/* Disable I3C interface */
	lsm6dsox_i3c_disable_set(&g_dev_ctx, LSM6DSOX_I3C_DISABLE);

	/* Set XL Output Data Rate to 417 Hz */
	lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_417Hz);

	/* Set 2g full XL scale */
	lsm6dsox_xl_full_scale_set(&g_dev_ctx, LSM6DSOX_2g);

	/* Enable Tap detection on X, Y, Z */
	lsm6dsox_tap_detection_on_z_set(&g_dev_ctx, PROPERTY_ENABLE);
	lsm6dsox_tap_detection_on_y_set(&g_dev_ctx, PROPERTY_ENABLE);
	lsm6dsox_tap_detection_on_x_set(&g_dev_ctx, PROPERTY_ENABLE);

	/* Set Tap threshold to 01000b, therefore the tap threshold
	 * is 500 mg (= 12 * FS_XL / 32 )
	 */
	lsm6dsox_tap_threshold_x_set(&g_dev_ctx, 0x08);
	lsm6dsox_tap_threshold_y_set(&g_dev_ctx, 0x08);
	lsm6dsox_tap_threshold_z_set(&g_dev_ctx, 0x08);

	/* Configure Single and Double Tap parameter
	 *
	 * For the maximum time between two consecutive detected taps, the DUR
	 * field of the INT_DUR2 register is set to 0111b, therefore the Duration
	 * time is 538.5 ms (= 7 * 32 * ODR_XL)
	 *
	 * The SHOCK field of the INT_DUR2 register is set to 11b, therefore
	 * the Shock time is 57.36 ms (= 3 * 8 * ODR_XL)
	 *
	 * The QUIET field of the INT_DUR2 register is set to 11b, therefore
	 * the Quiet time is 28.68 ms (= 3 * 4 * ODR_XL)
	 */
	lsm6dsox_tap_dur_set(&g_dev_ctx, 0x07);
	lsm6dsox_tap_quiet_set(&g_dev_ctx, 0x03);
	lsm6dsox_tap_shock_set(&g_dev_ctx, 0x03);

	/* Enable Single and Double Tap detection. */
	lsm6dsox_tap_mode_set(&g_dev_ctx, LSM6DSOX_BOTH_SINGLE_DOUBLE);

	/* For single tap only uncomments next function */
	//lsm6dsox_tap_mode_set(&dev_ctx, LSM6DSOX_ONLY_SINGLE);
	/* Enable interrupt generation on Single and Double Tap INT1 pin */
	//lsm6dsox_pin_int1_route_get(&dev_ctx, &int1_route);
	/* For single tap only comment next function */
	//int1_route.reg.md1_cfg.int1_double_tap = PROPERTY_ENABLE;
	//int1_route.reg.md1_cfg.int1_single_tap = PROPERTY_ENABLE;
	//lsm6dsox_pin_int1_route_set(&dev_ctx, &int1_route);
	/* Uncomment if interrupt generation on Single and Double Tap INT2 pin */
	lsm6dsox_pin_int2_route_get(&g_dev_ctx, NULL, &int2_route);

	/* For single tap only comment next function */
	int2_route.double_tap = PROPERTY_ENABLE;
	int2_route.single_tap = PROPERTY_ENABLE;
	lsm6dsox_pin_int2_route_set(&g_dev_ctx, NULL, int2_route);
}

/* Main Example --------------------------------------------------------------*/
void example_main_tilt_lsm6dsox(void)
{
	stmdev_ctx_t dev_ctx;

	/* Uncomment to configure INT 1 */
	//lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment to configure INT 2 */
	lsm6dsox_pin_int2_route_t int2_route;

	/* Initialize mems driver interface */
	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	//dev_ctx.handle = &hi2c1;

	/* Init test platform */
	platform_init();

	/* Wait sensor boot time */
	platform_delay(10);

	/* Check device ID */
	lsm6dsox_device_id_get(&dev_ctx, &whoamI);
	if (whoamI != LSM6DSOX_ID)
		while (1)
			;

	/* Restore default configuration */
	lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do
	{
		lsm6dsox_reset_get(&dev_ctx, &rst);
	} while (rst);

	/* Disable I3C interface */
	lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);

	/*
	 * Set XL Output Data Rate: The tilt function works at 26 Hz,
	 * so the accelerometer ODR must be set at 26 Hz or higher values
	 */
	lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_26Hz);

	/* Set 2g full XL scale. */
	lsm6dsox_xl_full_scale_set(&dev_ctx, LSM6DSOX_2g);

	/* Enable Tilt in embedded function. */
	lsm6dsox_tilt_sens_set(&dev_ctx, PROPERTY_ENABLE);

	/* Uncomment if interrupt generation on Tilt INT1 pin */
	//lsm6dsox_pin_int1_route_get(&dev_ctx, &int1_route);
	//int1_route.reg.emb_func_int1.int1_tilt = PROPERTY_ENABLE;
	//lsm6dsox_pin_int1_route_set(&dev_ctx, &int1_route);
	/* Uncomment if interrupt generation on Tilt INT2 pin */
	lsm6dsox_pin_int2_route_get(&dev_ctx, NULL, &int2_route);
	int2_route.tilt = PROPERTY_ENABLE;
	lsm6dsox_pin_int2_route_set(&dev_ctx, NULL, int2_route);

	/* Uncomment to have interrupt latched */
	//lsm6dsox_int_notification_set(&dev_ctx, PROPERTY_ENABLE);
	/* Wait Events. */
	while (1)
	{
		uint8_t is_tilt;

		/* Check if Tilt events */
		lsm6dsox_tilt_flag_data_ready_get(&dev_ctx, &is_tilt);
		if (is_tilt)
		{
			sprintf((char *)tx_buffer, "TILT Detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}
	}
}

void lsm6dsox_fifo_pedo_simple(void)
{
	stmdev_ctx_t ag_ctx;

	/* Uncomment to configure INT 1 */
	//lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment to configure INT 2 */
	lsm6dsox_pin_int2_route_t int2_route;
	ag_ctx.write_reg = platform_write;
	ag_ctx.read_reg = platform_read;
	//ag_ctx.handle = &hi2c1;

	/* Init test platform */
	platform_init();

	/* Wait sensor boot time */
	platform_delay(10);

	/* Check device ID */
	lsm6dsox_device_id_get(&ag_ctx, &whoamI);
	if (whoamI != LSM6DSOX_ID)
		while (1)
			;

	/* Restore default configuration */
	lsm6dsox_reset_set(&ag_ctx, PROPERTY_ENABLE);
	do
	{
		lsm6dsox_reset_get(&ag_ctx, &rst);
	} while (rst);

	/* Disable I3C interface */
	lsm6dsox_i3c_disable_set(&ag_ctx, LSM6DSOX_I3C_DISABLE);

	/* Set XL full scale */
	lsm6dsox_xl_full_scale_set(&ag_ctx, LSM6DSOX_2g);

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(&ag_ctx, PROPERTY_ENABLE);

	/* Set FIFO mode to Stream mode (aka Continuous Mode) */
	lsm6dsox_fifo_mode_set(&ag_ctx, LSM6DSOX_STREAM_MODE);

	/* Enable latched interrupt notification. */
	lsm6dsox_int_notification_set(&ag_ctx, LSM6DSOX_ALL_INT_LATCHED);

	/* Enable drdy 75 μs pulse: uncomment if interrupt must be pulsed. */
	//lsm6dsox_data_ready_mode_set(&ag_ctx, LSM6DSOX_DRDY_PULSED);
	/*
	 * FIFO watermark interrupt routed on INT1 pin
	 *
	 * Remember that INT1 pin is used by sensor to switch in I3C mode
	 * Uncomment to configure INT 1
	 */
	//lsm6dsox_pin_int1_route_get(&ag_ctx, &int1_route);
	//int1_route.reg.emb_func_int1.int1_step_detector = PROPERTY_ENABLE;
	//lsm6dsox_pin_int1_route_set(&ag_ctx, &int1_route);
	/*
	 * FIFO watermark interrupt routed on INT2 pin
	 * Uncomment to configure INT 2
	 */
	lsm6dsox_pin_int2_route_get(&ag_ctx, NULL, &int2_route);
	int2_route.step_detector = PROPERTY_ENABLE;

	lsm6dsox_pin_int2_route_set(&ag_ctx, NULL, int2_route);
	/* Enable HW Timestamp */
	lsm6dsox_timestamp_set(&ag_ctx, PROPERTY_ENABLE);

	/* Enable pedometer */
	lsm6dsox_pedo_sens_set(&ag_ctx, LSM6DSOX_PEDO_BASE_MODE);
	lsm6dsox_fifo_pedo_batch_set(&ag_ctx, PROPERTY_ENABLE);
	lsm6dsox_steps_reset(&ag_ctx);

	/* Set Output Data Rate */
	lsm6dsox_xl_data_rate_set(&ag_ctx, LSM6DSOX_XL_ODR_26Hz);

	while (1)
	{
		uint16_t num = 0;
		lsm6dsox_fifo_tag_t reg_tag;
		pedo_count_sample_t pedo_sample;

		/* Read FIFO samples number */
		lsm6dsox_fifo_data_level_get(&ag_ctx, &num);
		if (num > 0)
		{
			while (num--)
			{
				/* Read FIFO tag */
				lsm6dsox_fifo_sensor_tag_get(&ag_ctx, &reg_tag);
				switch (reg_tag)
				{
				case LSM6DSOX_STEP_CPUNTER_TAG:
					lsm6dsox_fifo_out_raw_get(&ag_ctx, pedo_sample.byte);

					sprintf((char *)tx_buffer, "Step Count :%u T %u\r\n",
							(unsigned int)pedo_sample.step_count,
							(unsigned int)pedo_sample.timestamp);
					tx_com(tx_buffer, strlen((char const *)tx_buffer));
					break;
				default:
					break;
				}
			}
		}
	}
}

void lsm6dsox_read_data_simple(void)
{
	stmdev_ctx_t dev_ctx;

	/* Initialize mems driver interface */
	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	//dev_ctx.handle = &hi2c1;

	/* Init test platform */
	platform_init();

	/* Wait sensor boot time */
	platform_delay(10);

	/* Check device ID */
	lsm6dsox_device_id_get(&dev_ctx, &whoamI);
	if (whoamI != LSM6DSOX_ID)
		while (1)
			;

	/* Restore default configuration */
	lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do
	{
		lsm6dsox_reset_get(&dev_ctx, &rst);
	} while (rst);

	/* Disable I3C interface */
	lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

	/* Set Output Data Rate */
	lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_12Hz5);
	lsm6dsox_gy_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_12Hz5);

	/* Set full scale */
	lsm6dsox_xl_full_scale_set(&dev_ctx, LSM6DSOX_2g);
	lsm6dsox_gy_full_scale_set(&dev_ctx, LSM6DSOX_2000dps);

	/*
	 * Configure filtering chain(No aux interface)
	 *
	 * Accelerometer - LPF1 + LPF2 path
	 */
	lsm6dsox_xl_hp_path_on_out_set(&dev_ctx, LSM6DSOX_LP_ODR_DIV_100);
	lsm6dsox_xl_filter_lp2_set(&dev_ctx, PROPERTY_ENABLE);

	/* Read samples in polling mode (no int) */
	while (1)
	{
		platform_delay(500);
		uint8_t reg;

		/* Read output only if new xl value is available */

		lsm6dsox_xl_flag_data_ready_get(&dev_ctx, &reg);
		if (reg)
		{
			// Read acceleration field data
			memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
			lsm6dsox_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
			acceleration_mg[0] = lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[0]);
			acceleration_mg[1] = lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[1]);
			acceleration_mg[2] = lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[2]);

			sprintf((char *)tx_buffer, "Acceleration [mg]: %d | %d | %d\r\n",
					(int)(acceleration_mg[0]),
					(int)(acceleration_mg[1]),
					(int)(acceleration_mg[2]));
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		lsm6dsox_gy_flag_data_ready_get(&dev_ctx, &reg);
		if (reg)
		{
			// Read angular rate field data
			memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
			lsm6dsox_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
			angular_rate_mdps[0] = lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0]);
			angular_rate_mdps[1] = lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1]);
			angular_rate_mdps[2] = lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2]);

			sprintf((char *)tx_buffer, "Angular rate [mdps]: %d | %d | %d\r\n",
					(int)(angular_rate_mdps[0]),
					(int)(angular_rate_mdps[1]),
					(int)(angular_rate_mdps[2]));
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		/*
		 lsm6dsox_temp_flag_data_ready_get(&dev_ctx, &reg);
		 if (reg) {
		 // Read temperature data
		 memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
		 lsm6dsox_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
		 temperature_degC = lsm6dsox_from_lsb_to_celsius(data_raw_temperature.i16bit);

		 sprintf((char*) tx_buffer, "Temperature [degC]: %d\r\n", (int)temperature_degC);
		 tx_com(tx_buffer, strlen((char const*) tx_buffer));
		 }
		 */
	}
}

void lsm6dsox_fsm(void)
{
	/* Variable declaration */
	stmdev_ctx_t dev_ctx;
	lsm6dsox_pin_int1_route_t pin_int1_route;
	lsm6dsox_emb_fsm_enable_t fsm_enable;
	lsm6dsox_fsm_out_t fsm_out;
	lsm6dsox_all_sources_t status;
	uint16_t fsm_addr;

	/* Initialize mems driver interface */
	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	//dev_ctx.handle = &hi2c1;

	/* Wait sensor boot time */
	platform_delay(10);

	/* Check device ID */
	lsm6dsox_device_id_get(&dev_ctx, &whoamI);
	if (whoamI != LSM6DSOX_ID)
		while (1)
			;

	/* Restore default configuration (not FSM) */
	lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
	do
	{
		lsm6dsox_reset_get(&dev_ctx, &rst);
	} while (rst);

	/* Disable I3C interface */
	lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

	/* Set full scale */
	lsm6dsox_xl_full_scale_set(&dev_ctx, LSM6DSOX_2g);
	lsm6dsox_gy_full_scale_set(&dev_ctx, LSM6DSOX_2000dps);

	/* Route signals on interrupt pin 1 */
	lsm6dsox_pin_int1_route_get(&dev_ctx, &pin_int1_route);
	pin_int1_route.fsm1 = PROPERTY_ENABLE;
	pin_int1_route.fsm2 = PROPERTY_ENABLE;
	pin_int1_route.fsm3 = PROPERTY_ENABLE;
	pin_int1_route.fsm4 = PROPERTY_ENABLE;
	pin_int1_route.fsm5 = PROPERTY_ENABLE;
	pin_int1_route.fsm6 = PROPERTY_ENABLE;
	pin_int1_route.fsm7 = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(&dev_ctx, pin_int1_route);

	/* Configure interrupt pin mode notification */
	lsm6dsox_int_notification_set(&dev_ctx, LSM6DSOX_BASE_PULSED_EMB_LATCHED);

	/*
	 * Start Finite State Machine configuration
	 */

	/* Reset Long Counter */
	lsm6dsox_long_cnt_int_value_set(&dev_ctx, 0x0000U);

	/* Set the first address where the programs are written */
	lsm6dsox_fsm_start_address_set(&dev_ctx, LSM6DSOX_START_FSM_ADD);

	/* Set the number of the programs */
	lsm6dsox_fsm_number_of_programs_set(&dev_ctx, 7);

	/* Enable final state machine */
	fsm_enable.fsm_enable_a.fsm1_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm2_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm3_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm4_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm5_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm6_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm7_en = PROPERTY_ENABLE;
	fsm_enable.fsm_enable_a.fsm8_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm9_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm10_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm11_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm12_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm13_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm14_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm15_en = PROPERTY_DISABLE;
	fsm_enable.fsm_enable_b.fsm16_en = PROPERTY_DISABLE;
	lsm6dsox_fsm_enable_set(&dev_ctx, &fsm_enable);

	/* Set Finite State Machine data rate */
	lsm6dsox_fsm_data_rate_set(&dev_ctx, LSM6DSOX_ODR_FSM_26Hz);

	/* Write Programs */
	fsm_addr = LSM6DSOX_START_FSM_ADD;

	/* Glance */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_glance,
						 sizeof(lsm6so_prg_glance));
	fsm_addr += sizeof(lsm6so_prg_glance);

	/* motion */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_motion,
						 sizeof(lsm6so_prg_motion));
	fsm_addr += sizeof(lsm6so_prg_motion);

	/* no_motion */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_no_motion,
						 sizeof(lsm6so_prg_no_motion));
	fsm_addr += sizeof(lsm6so_prg_no_motion);

	/* wakeup */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_wakeup,
						 sizeof(lsm6so_prg_wakeup));
	fsm_addr += sizeof(lsm6so_prg_wakeup);

	/* pickup */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_pickup,
						 sizeof(lsm6so_prg_pickup));
	fsm_addr += sizeof(lsm6so_prg_pickup);

	/* orientation */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_orientation,
						 sizeof(lsm6so_prg_orientation));
	fsm_addr += sizeof(lsm6so_prg_orientation);

	/* wrist_tilt */
	lsm6dsox_ln_pg_write(&dev_ctx, fsm_addr, (uint8_t *)lsm6so_prg_wrist_tilt,
						 sizeof(lsm6so_prg_wrist_tilt));

	/*
	 * End Finite State Machine configuration
	 */

	/* Set Output Data Rate */
	lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_104Hz);
	lsm6dsox_gy_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_104Hz);

	sprintf((char *)tx_buffer, "Main loop:\r\n");
	tx_com(tx_buffer, strlen((char const *)tx_buffer));

	/* Main loop */
	while (1)
	{

		sprintf((char *)tx_buffer, "Tick...\r\n");
		tx_com(tx_buffer, strlen((char const *)tx_buffer));

		/* Read interrupt source registers in polling mode (no int) */
		lsm6dsox_all_sources_get(&dev_ctx, &status);

		if (status.fsm1)
		{
			sprintf((char *)tx_buffer, "glance detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		if (status.fsm2)
		{
			sprintf((char *)tx_buffer, "motion detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		if (status.fsm3)
		{
			sprintf((char *)tx_buffer, "no motion detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		if (status.fsm4)
		{
			sprintf((char *)tx_buffer, "wakeup detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		if (status.fsm5)
		{
			sprintf((char *)tx_buffer, "pickup detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		if (status.fsm6)
		{
			lsm6dsox_fsm_out_get(&dev_ctx, &fsm_out);
			sprintf((char *)tx_buffer, "orientation detected (%d, %d, %d, %d, %d, %d, %d, %d)\r\n",
					fsm_out.fsm_outs6.n_v, fsm_out.fsm_outs6.p_v, fsm_out.fsm_outs6.n_z,
					fsm_out.fsm_outs6.p_z, fsm_out.fsm_outs6.n_y, fsm_out.fsm_outs6.p_y,
					fsm_out.fsm_outs6.n_x, fsm_out.fsm_outs6.p_x);
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}

		if (status.fsm7)
		{
			sprintf((char *)tx_buffer, "wrist tilt detected\r\n");
			tx_com(tx_buffer, strlen((char const *)tx_buffer));
		}
		platform_delay(20);
	}
}

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
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
#ifdef LSM6DSOX_USE_I2C
	imu_i2c_write((uint8_t)LSM6DSOX_I2C_ADD_L >> 1, reg, bufp, len);
#else
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, &reg, 1, NULL, 0));
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, bufp, len, NULL, 0));
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
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
#ifdef LSM6DSOX_USE_I2C
	imu_i2c_read((uint8_t)LSM6DSOX_I2C_ADD_L >> 1, reg, bufp, len);
#else
	/* Read command */
	reg |= 0x80;
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, &reg, 1, bufp, len));
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
static void tx_com(uint8_t *tx_buffer, uint16_t len)
{
	NRF_LOG_INFO("%s", tx_buffer);
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
static void platform_delay(uint32_t ms)
{
	nrf_delay_ms(ms);
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
static void platform_init(void)
{
	ret_code_t err_code;

	/*
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    //nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    //nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    err_code = nrf_drv_gpiote_in_init(LSM6DSOX_INT_PIN, &in_config, interrupt_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(LSM6DSOX_INT_PIN, true);
	*/

	/**************************************************************************/

#ifdef LSM6DSOX_USE_I2C
	imu_i2c_init();
#else
	nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

	spi_config.sck_pin = LSM6DSOX_SPI_SCK_PIN;
	spi_config.miso_pin = LSM6DSOX_SPI_MISO_PIN;
	spi_config.mosi_pin = LSM6DSOX_SPI_MOSI_PIN;
	spi_config.ss_pin = LSM6DSOX_SPI_SS_PIN;

	err_code = nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
	return err_code;
#endif

	int2_pin_init();
}

static void int2_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	lsm6dsox_all_sources_t all_source;

	/* Check if Tap events */

	lsm6dsox_all_sources_get(&g_dev_ctx, &all_source);
	if (all_source.double_tap)
	{
		sprintf((char *)tx_buffer, "D-Tap: ");
		if (all_source.tap_x)
			strcat((char *)tx_buffer, "x-axis");
		else if (all_source.tap_x)
			strcat((char *)tx_buffer, "y-axis");
		else
			strcat((char *)tx_buffer, "z-axis");
		if (all_source.tap_sign)
			strcat((char *)tx_buffer, " negative");
		else
			strcat((char *)tx_buffer, " positive");
		strcat((char *)tx_buffer, " sign\r\n");
		tx_com(tx_buffer, strlen((char const *)tx_buffer));
	}

	if (all_source.single_tap)
	{
		sprintf((char *)tx_buffer, "S-Tap: ");
		if (all_source.tap_x)
			strcat((char *)tx_buffer, "x-axis");
		else if (all_source.tap_y)
			strcat((char *)tx_buffer, "y-axis");
		else
			strcat((char *)tx_buffer, "z-axis");
		if (all_source.tap_sign)
			strcat((char *)tx_buffer, " negative");
		else
			strcat((char *)tx_buffer, " positive");
		strcat((char *)tx_buffer, " sign\r\n");
		tx_com(tx_buffer, strlen((char const *)tx_buffer));
	}
}

static void int2_pin_init(void)
{
	ret_code_t err_code;

	if (!nrf_drv_gpiote_is_init())
	{
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}

	//nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);

	//in_config.pull = NRF_GPIO_PIN_PULLUP;

	err_code = nrf_drv_gpiote_in_init(LSM6DSOX_INT2, &in_config, int2_pin_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(LSM6DSOX_INT2, true);
}