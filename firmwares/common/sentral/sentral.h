#ifndef SENTRAL_H
#define SENTRAL_H

#include "controller_math.h"
#include "stm32h7xx_hal.h"

void sentral_device_info(void);
void sentral_feature_info(void);
void sentral_EEPROM_upload(void);
void sentral_config_operating_mode(void);
void sentral_read_status(void);
void sentral_data(quaternion_t *quat);
void sentral_init(I2C_HandleTypeDef *dev);

#endif
