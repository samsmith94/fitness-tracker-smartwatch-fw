#ifndef I2C_H
#define I2C_H

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define TWI_INSTANCE_ID 0

void i2c_init(void);
void i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size);
void i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buff, uint16_t size);
void i2c_send(uint8_t slave_addr, uint8_t command, uint8_t value);

#endif