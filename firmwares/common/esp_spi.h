#ifndef SPI_H
#define SPI_H

#include "stm32h7xx_hal.h"

#define SPI_DATA_RX_SIZE 4

extern uint8_t spi_data_rx[];

void esp_spi_init(SPI_HandleTypeDef *dev);

#endif
