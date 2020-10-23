#include "lsm6dsox_tap.h"

void lsm6dsox_tap_init(void)
{
	/* Uncomment to configure INT 1 */
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
	//lsm6dsox_tap_mode_set(&g_dev_ctx, LSM6DSOX_ONLY_SINGLE);
	/* Enable interrupt generation on Single and Double Tap INT1 pin */
	//lsm6dsox_pin_int1_route_get(&g_dev_ctx, &int1_route);
	/* For single tap only comment next function */
	//int1_route.double_tap = PROPERTY_ENABLE;
	//int1_route.single_tap = PROPERTY_ENABLE;
	//lsm6dsox_pin_int1_route_set(&g_dev_ctx, int1_route);
	/* Uncomment if interrupt generation on Single and Double Tap INT2 pin */
	lsm6dsox_pin_int2_route_get(&g_dev_ctx, NULL, &int2_route);

	/* For single tap only comment next function */
	int2_route.double_tap = PROPERTY_ENABLE;
	int2_route.single_tap = PROPERTY_ENABLE;
	lsm6dsox_pin_int2_route_set(&g_dev_ctx, NULL, int2_route);
}


void lsm6dsox_double_tap_irq_handler(void)
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