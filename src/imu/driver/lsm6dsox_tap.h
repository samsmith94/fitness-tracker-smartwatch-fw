#ifndef LSM6DSOX_TAP_H
#define LSM6DSOX_TAP_H

#include "lsm6dsox_reg.h"
#include "platform.h"

void lsm6dsox_tap_init(void);
void lsm6dsox_double_tap_irq_handler(void);

#endif