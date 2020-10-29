#include "lsm6dsox_read_data.h"

static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis1bit16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;

void lsm6dsox_read_data_init(void)
{
	/* Uncomment to configure INT */
	//lsm6dsox_pin_int1_route_t int1_route;

	/* Uncomment to configure INT 2 */
	lsm6dsox_pin_int2_route_t int2_route;

	/* Initialize mems driver interface */
	g_dev_ctx.write_reg = platform_write;
	g_dev_ctx.read_reg = platform_read;
	g_dev_ctx.handle = &imu_m_twi;

	/* Init test platform */
	platform_init();

	/* Wait sensor boot time */
	platform_delay(10);

	/* Check device ID */
	lsm6dsox_device_id_get(&g_dev_ctx, &whoamI);
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

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);

	/* Set Output Data Rate */
	lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_12Hz5);
	lsm6dsox_gy_data_rate_set(&g_dev_ctx, LSM6DSOX_GY_ODR_12Hz5);

	/* Set full scale */
	lsm6dsox_xl_full_scale_set(&g_dev_ctx, LSM6DSOX_2g);
	lsm6dsox_gy_full_scale_set(&g_dev_ctx, LSM6DSOX_2000dps);

	/* Enable drdy 75 μs pulse: uncomment if interrupt must be pulsed */
	lsm6dsox_data_ready_mode_set(&g_dev_ctx, LSM6DSOX_DRDY_PULSED);

	/* Uncomment if interrupt generation on Free Fall INT1 pin */
	//lsm6dsox_pin_int1_route_get(&g_dev_ctx, &int1_route);
	//int1_route.drdy_g = PROPERTY_ENABLE;
	//int1_route.drdy_xl = PROPERTY_ENABLE;
	//lsm6dsox_pin_int1_route_set(&g_dev_ctx, int1_route);

	/* Uncomment if interrupt generation on Free Fall INT2 pin */
	lsm6dsox_pin_int2_route_get(&g_dev_ctx, NULL, &int2_route);
	int2_route.free_fall = PROPERTY_ENABLE;
	int2_route.drdy_g = PROPERTY_ENABLE;
	int2_route.drdy_xl = PROPERTY_ENABLE;
	lsm6dsox_pin_int2_route_set(&g_dev_ctx, NULL, int2_route);
}

void lsm6dsox_read_data_irq_handler(void)
{
	uint8_t reg;

	/* Read output only if new xl value is available */
	lsm6dsox_xl_flag_data_ready_get(&g_dev_ctx, &reg);
	if (reg)
	{
		/* Read acceleration field data */
		memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
		lsm6dsox_acceleration_raw_get(&g_dev_ctx, data_raw_acceleration.u8bit);
		acceleration_mg[0] =
			lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[0]);
		acceleration_mg[1] =
			lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[1]);
		acceleration_mg[2] =
			lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[2]);

		sprintf((char *)tx_buffer, "Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
				acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
		tx_com(tx_buffer, strlen((char const *)tx_buffer));
	}

	/* Read output only if new gyro value is available */
	lsm6dsox_gy_flag_data_ready_get(&g_dev_ctx, &reg);
	if (reg)
	{
		/* Read angular rate field data */
		memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
		lsm6dsox_angular_rate_raw_get(&g_dev_ctx, data_raw_angular_rate.u8bit);
		angular_rate_mdps[0] =
			lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0]);
		angular_rate_mdps[1] =
			lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1]);
		angular_rate_mdps[2] =
			lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2]);

		sprintf((char *)tx_buffer, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
				angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
		tx_com(tx_buffer, strlen((char const *)tx_buffer));
	}
}
