#include "controller.h"
#include "controller_math.h"
#include "sentral.h"
#include "esp_spi.h"
#include "sk9822strip.h"
#include "spi.h"
#include "cmsis_os.h"
#include "SEGGER_RTT.h"
#include "main_task.h"
#include "stm32h7xx_hal.h"

#define NB_TRI_IN_CHAIN 20
#define NB_LEDS_IN_TRI 78
#define NB_LEDS (NB_TRI_IN_CHAIN*NB_LEDS_IN_TRI)

DECLLEDBUF(buf, NB_LEDS);

void main_task(const void* arg) {
	SPI_HandleTypeDef *spi_strip_dev_1 = ((struct main_task_args*) arg)->spi_strip_dev_1;
	SPI_HandleTypeDef *spi_esp_dev = ((struct main_task_args*) arg)->spi_esp_dev;
	I2C_HandleTypeDef *i2c_dev = ((struct main_task_args*) arg)->i2c_dev;

	SEGGER_RTT_printf(0, "Main task\n");
	
	sentral_init(i2c_dev);
	SEGGER_RTT_printf(0, "Sentral init'd\n");
	
	esp_spi_init(spi_esp_dev);
	SEGGER_RTT_printf(0, "ESP SPI init'd\n");

	struct frame* leds = stripbuf_init(buf, NB_LEDS);
	SEGGER_RTT_printf(0, "Strip buffer init'd\n");
	
	// Send the top 5 triangles first, for testing
	struct frame* triangles[20];
	const int order[20] = {18, 0, 19, 1, 15, 10, 11, 12, 14, 2, 16, 3, 17, 4, 13,    5, 6, 7, 8, 9};
	for (int i = 0; i < 20; i++)
		triangles[order[i]] =  &leds[i*NB_LEDS_IN_TRI];

	quaternion_t rotation = {1, 0, 0, 0};
	uint32_t last_time = osKernelSysTick();
	int counter = 0;
	while (1)
	{
		sentral_data(&rotation);
		quaternion_t rotation_permuted = {
			.w = rotation.w,
			.x = rotation.y,
			.y = rotation.z,
			.z = rotation.x
		};
		compute_led_colors(triangles, rotation_permuted);
		stripbuf_send(spi_strip_dev_1, buf, NB_LEDS);
		
		// Compute and display fps
		counter++;
		if (counter == 200) {
			uint32_t time = osKernelSysTick();
			float fps = ((float) counter) * ((float) osKernelSysTickFrequency) / ((float) (time - last_time));
			last_time = time;
			SEGGER_RTT_printf(0, "fps=%d\n", (int) fps);
			counter = 0;
		}
	}
}
