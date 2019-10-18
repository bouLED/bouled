#include "sk9822strip.h"
#include "stdint.h"

#ifndef SIMULATION
#include "SEGGER_RTT.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

struct frame* stripbuf_init(uint8_t* buf, int nbleds)
{
	struct frame* frames = (struct frame*) buf;
	/* Skip first frame, it should be zeroes */
	frames++;
#ifndef SIMULATION
	set_brightness(frames, nbleds, 1);
#else
	(void) nbleds;
#endif

	return frames;
}

#ifndef SIMULATION

void stripbuf_send(SPI_HandleTypeDef* dev, uint8_t* buf, int nbleds)
{
	HAL_StatusTypeDef x = HAL_SPI_Transmit(dev, buf, BUFSIZE(nbleds),
			HAL_MAX_DELAY);
	if (x != HAL_OK) {
		char* stat;
		switch (x) {
			case HAL_ERROR:
			default:
				stat = "ERROR";
				break;
			case HAL_BUSY:
				stat = "BUSY";
				break;
			case HAL_TIMEOUT:
				stat = "TIMEOUT";
				break;
		}
		SEGGER_RTT_printf(0, "SPI send status: %s\n", stat);
	}
}

void test_chain(uint8_t* buf, int nbleds, SPI_HandleTypeDef* dev, int brightness)
{
	struct frame* leds = stripbuf_init(buf, nbleds);
	set_brightness(leds, nbleds, brightness);

	for (int i = 0; i < nbleds; i++) {
		leds[i].r = 0xff;
		leds[i].g = 0xff;
		leds[i].b = 0xff;
	}

	stripbuf_send(dev, buf, nbleds);
	vTaskDelay(500/portTICK_PERIOD_MS);

	for (int i = 0; i < nbleds; i++) {
		leds[i].r = 0;
		leds[i].g = 0;
		leds[i].b = 0;
	}

	for (int i = 0;; i = (i+1) % nbleds) {
		leds[i].r = 0xff;
		leds[(i+1) % nbleds].g = 0xff;
		leds[(i+2) % nbleds].b = 0xff;

		stripbuf_send(dev, buf, nbleds);

		leds[i].r = 0;
		leds[(i+1) % nbleds].g = 0;
		leds[(i+2) % nbleds].b = 0;
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

void set_brightness(struct frame* frames, int nbleds, int brightness)
{
	if (brightness < 0 || brightness > 31) {
		SEGGER_RTT_printf(0, "LED brightness out of bounds: %d\n",
				brightness);
		return;
	}

	for (int i = 0; i < nbleds; i++)
		frames[i]._pad = 0xe0 + brightness;
}

#endif
