#ifndef LSM6DSOX_MULTI_CONF_H
#define LSM6DSOX_MULTI_CONF_H

#include "lsm6dsox_reg.h"
#include "platform.h"

void lsm6dsox_multi_conf_init(void);
void lsm6dsox_multi_conf_irq_handler(void);


extern const ucf_line_t lsm6dsox_yoga_pose_recognition[];

void lsm6dsox_fsm_init(void);
void lsm6dsox_fsm_irq_handler(void);


void fsm_multiconf_iqr_handler(void);
void fsm_multiconf_init(void);

#endif