#include "stm32h7xx_hal.h"
#include "SEGGER_RTT.h"
#include "esp_spi.h"

uint8_t spi_data_rx[SPI_DATA_RX_SIZE];

void esp_spi_init(SPI_HandleTypeDef *dev)
{
	HAL_SPI_Receive_IT(dev, spi_data_rx, SPI_DATA_RX_SIZE);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SEGGER_RTT_printf(0, "ESP32 sent %d %d %d %d\n", (int) spi_data_rx[0], (int) spi_data_rx[1], (int) spi_data_rx[2], (int) spi_data_rx[3]);
	HAL_SPI_Receive_IT(hspi, spi_data_rx, SPI_DATA_RX_SIZE);
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	uint32_t error = HAL_SPI_GetError(hspi);
	if (error & HAL_SPI_ERROR_MODF)
			SEGGER_RTT_printf(0, "SPI error modf\n");
	if (error & HAL_SPI_ERROR_CRC)
			SEGGER_RTT_printf(0, "SPI error crc\n");
	if (error & HAL_SPI_ERROR_OVR)
			SEGGER_RTT_printf(0, "SPI error ovr\n");
	if (error & HAL_SPI_ERROR_FRE)
			SEGGER_RTT_printf(0, "SPI error fre\n");
	if (error & HAL_SPI_ERROR_DMA)
			SEGGER_RTT_printf(0, "SPI error dma\n");
	if (error & HAL_SPI_ERROR_FLAG)
			SEGGER_RTT_printf(0, "SPI error flag: %d\n", (int) error, HAL_SPI_ERROR_FLAG);

	HAL_SPI_Receive_IT(hspi, spi_data_rx, SPI_DATA_RX_SIZE);
}
