#ifndef LSM6DSOX_FIFO_PEDO_H
#define LSM6DSOX_FIFO_PEDO_H

#include "lsm6dsox_reg.h"
#include "platform.h"

void lsm6dsox_fifo_pedo_init(void);
void lsm6dsox_read_steps(void);

void lsm6dsox_fifo_pedo_irq_handler(void);


#endif