#ifndef LSM6DSOX_READ_DATA_H
#define LSM6DSOX_READ_DATA_H

#include "lsm6dsox_reg.h"
#include "platform.h"

void lsm6dsox_read_data_init(void);
void lsm6dsox_read_data_irq_handler(void);

#endif