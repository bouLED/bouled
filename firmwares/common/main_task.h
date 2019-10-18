#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "stm32h7xx_hal.h"

void main_task(const void* arg);

struct main_task_args
{
	SPI_HandleTypeDef *spi_strip_dev_1;
	SPI_HandleTypeDef *spi_esp_dev;
	I2C_HandleTypeDef *i2c_dev;
};

#endif
