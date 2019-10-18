#include "spi.h"
#include "led_strip.h"
#include "sk9822strip.h"

#include "SEGGER_RTT.h"

#define NB_TRI_IN_CHAIN 5
#define BRIGHTNESS 1
#define NB_LEDS_IN_TRI 78
#define NB_LEDS (NB_TRI_IN_CHAIN*NB_LEDS_IN_TRI)

DECLLEDBUF(buf, NB_LEDS);

/* Simple test animation */
void led_task(void *pv)
{
	test_chain(buf, NB_LEDS, &hspi5, BRIGHTNESS);
}
