#include "lsm6dsox_fifo_pedo.h"

uint16_t step_count;

void lsm6dsox_fifo_pedo_init(void)
{
	lsm6dsox_emb_sens_t emb_sens;

	/* Uncomment to configure INT 1 */
	lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment to configure INT 2 */
	lsm6dsox_pin_int2_route_t int2_route;

#if 0
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
#endif

	/* Set XL full scale */
	lsm6dsox_xl_full_scale_set(&g_dev_ctx, LSM6DSOX_2g);

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);

	/* Set FIFO mode to Stream mode (aka Continuous Mode) */
	//lsm6dsox_fifo_mode_set(&g_dev_ctx, LSM6DSOX_STREAM_MODE);

	/* Enable latched interrupt notification. */
	//lsm6dsox_int_notification_set(&g_dev_ctx, LSM6DSOX_ALL_INT_LATCHED);
	//lsm6dsox_int_notification_set(&g_dev_ctx, LSM6DSOX_ALL_INT_PULSED);

	/* Enable drdy 75 μs pulse: uncomment if interrupt must be pulsed. */
	//lsm6dsox_data_ready_mode_set(&g_dev_ctx, LSM6DSOX_DRDY_PULSED);
	//lsm6dsox_data_ready_mode_set(&g_dev_ctx, LSM6DSOX_DRDY_LATCHED);

	//lsm6dsox_pedo_int_mode_set(&g_dev_ctx, PROPERTY_ENABLE);
	/*
	 * FIFO watermark interrupt routed on INT1 pin
	 *
	 * Remember that INT1 pin is used by sensor to switch in I3C mode
	 * Uncomment to configure INT 1
	 */
	//lsm6dsox_pin_int1_route_get(&g_dev_ctx, &int1_route);
	//int1_route.step_detector = PROPERTY_ENABLE;
	//lsm6dsox_pin_int1_route_set(&g_dev_ctx, int1_route);
	/*
	 * FIFO watermark interrupt routed on INT2 pin
	 * Uncomment to configure INT 2
	 */
	//lsm6dsox_pin_int2_route_get(&g_dev_ctx, NULL, &int2_route);
	//int2_route.step_detector = PROPERTY_ENABLE;
	/*int2_route.fifo_full = PROPERTY_ENABLE;
	int2_route.fifo_bdr = PROPERTY_ENABLE;
	int2_route.fifo_ovr = PROPERTY_ENABLE;
	int2_route.fifo_th = PROPERTY_ENABLE;*/
	//lsm6dsox_pin_int2_route_set(&g_dev_ctx, NULL, int2_route);

	/* Enable HW Timestamp */
	//lsm6dsox_timestamp_set(&g_dev_ctx, PROPERTY_ENABLE);

	/* Enable pedometer */
	lsm6dsox_pedo_sens_set(&g_dev_ctx, LSM6DSOX_PEDO_BASE_MODE);
	emb_sens.step = PROPERTY_ENABLE;
	lsm6dsox_embedded_sens_set(&g_dev_ctx, &emb_sens);
	//lsm6dsox_fifo_pedo_batch_set(&g_dev_ctx, PROPERTY_ENABLE);
	lsm6dsox_steps_reset(&g_dev_ctx);

	/* Set Output Data Rate */
	//lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_26Hz);
	lsm6dsox_xl_data_rate_set(&g_dev_ctx, LSM6DSOX_XL_ODR_417Hz);


	/*
	while (1)
	{
		step_count = lsm6dsox_read_steps();
		nrf_delay_ms(1000);
	}
	*/
}

void lsm6dsox_read_steps(void)
{
	uint16_t steps;
	/* Read steps */
	lsm6dsox_number_of_steps_get(&g_dev_ctx, (uint8_t *)&steps);
	sprintf((char *)tx_buffer, "Steps :%d\r\n", step_count);
	tx_com(tx_buffer, strlen((char const *)tx_buffer));
	
	step_count = steps;
}


void lsm6dsox_fifo_pedo_irq_handler(void)
{
	uint16_t num = 0;
	lsm6dsox_fifo_tag_t reg_tag;
	pedo_count_sample_t pedo_sample;

	/* Read FIFO samples number */
	lsm6dsox_fifo_data_level_get(&g_dev_ctx, &num);
	if (num > 0)
	{
		while (num--)
		{
			/* Read FIFO tag */
			lsm6dsox_fifo_sensor_tag_get(&g_dev_ctx, &reg_tag);
			switch (reg_tag)
			{
			case LSM6DSOX_STEP_CPUNTER_TAG:
				lsm6dsox_fifo_out_raw_get(&g_dev_ctx, pedo_sample.byte);

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