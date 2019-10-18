#include "stm32l4xx_hal.h"
#include "SEGGER_RTT.h"
#include "spi.h"

uint8_t spi_data_rx[SPI_DATA_RX_SIZE];

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SEGGER_RTT_printf(0, "ESP32 sent %d %d %d %d\n", (int) spi_data_rx[0], (int) spi_data_rx[1], (int) spi_data_rx[2], (int) spi_data_rx[3]);
	HAL_SPI_Receive_IT(&hspi1, spi_data_rx, SPI_DATA_RX_SIZE);
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
	if (error & HAL_SPI_ERROR_FLAG) {
			SEGGER_RTT_printf(0, "SPI error flag: %d\n", (int) error, HAL_SPI_ERROR_FLAG);
			SEGGER_RTT_printf(0, "\t BSY = %d\n", __HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY));
			SEGGER_RTT_printf(0, "\t RXNE = %d\n", __HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE));
			SEGGER_RTT_printf(0, "\t TXE = %d\n", __HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE));
	}
	
	/*__HAL_SPI_CLEAR_CRCERRFLAG(&hspi);
	__HAL_SPI_CLEAR_MODFFLAG(&hspi);
	__HAL_SPI_CLEAR_OVRFLAG(&hspi);
	__HAL_SPI_CLEAR_FREFLAG(&hspi);*/

	HAL_SPI_Receive_IT(&hspi1, spi_data_rx, SPI_DATA_RX_SIZE);
}
