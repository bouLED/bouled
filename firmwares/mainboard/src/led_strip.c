#include "FreeRTOS.h"
#include "task.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"

#include "led_strip.h"
#include "sk9822strip.h"

#include "spi.h"
#include "SEGGER_RTT.h"

#define TEST_BRIGHTNESS 1

#define NB_CHAINS 4
#define NB_LEDSPERCHAIN (NB_TRIANGLES*NB_LEDSPERTRI/NB_CHAINS)
DECLLEDCHAINS(tab, NB_LEDSPERCHAIN, NB_CHAINS);

/* Usage: triangles[triangle number][led number] */
struct frame* triangles[NB_TRIANGLES];

/* What chain is this triangle in ? */
int chain[NB_TRIANGLES];

// Where is it in the chain ?
int triangle_number[NB_TRIANGLES];

/* Set triangle i to be the tri-th triangle in chain ch */
#define POSITION(i, ch, tri) do { \
	chain[i] = ch; \
	triangle_number[i] = tri; \
} while(0)

void led_strip_init(void)
{
	struct frame* chainstarts[NB_CHAINS];
	/* TODO: Set POSITIONs here */

	for (int i = 0; i < NB_CHAINS; i++)
		chainstarts[i] = stripbuf_init(tab[i], NB_LEDSPERCHAIN);

	for (int i = 0; i < NB_TRIANGLES; i++) {
		struct frame* c = chainstarts[chain[i]];
		int tri_pos = triangle_number[i]*NB_LEDSPERTRI;
		triangles[i] = &c[tri_pos];
	}
}

/* Simple test animation: test the first chain. */
void led_task(void *pv)
{
	test_chain(tab[0], NB_LEDSPERCHAIN, &hspi1, TEST_BRIGHTNESS);
}
