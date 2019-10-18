#ifndef SK9822STRIP_H
#define SK9822STRIP_H

#include "stdint.h"

#ifdef STM32H743xx
#include "stm32h7xx_hal.h"
#elif defined(STM32F746xx)
#include "stm32f7xx_hal.h"
#endif

/* Each LED delays the data by half a clock period,
   so this adds enough clock cycles to compensate .
 */
#define LEDFRAME_LEN 4
#define ENDFRAME_LEN(nb) ((nb/16)+2)
#define BUFSIZE(nb) LEDFRAME_LEN*(1+nb) + ENDFRAME_LEN(nb)

#define DECLLEDBUF(name, nbleds) static uint8_t name[BUFSIZE(nbleds)]
#define DECLLEDCHAINS(name, nbleds, nbchains) \
	static uint8_t name[nbchains][BUFSIZE(nbleds)]

/* Declare buf using DECLLEDBUF first.
 * Return value: first LED frame
 */
struct frame* stripbuf_init(uint8_t* buf, int nbleds);

struct frame {
    uint8_t _pad; /* DO NOT TOUCH */
    uint8_t b, g, r;
};

#ifndef SIMULATION

void stripbuf_send(SPI_HandleTypeDef* dev, uint8_t* buf, int nbleds);

/* Warning: brightness should be between 0 and 31, both included
 * VERY DANGEROUS, *never* set a brightness that's so high that the
 * LEDs draw too much current from the power supplies. Default value: 1.*/
void set_brightness(struct frame* frames, int nbleds, int brightness);

/* Test a chain of nbleds by showing a simple animation on it. Pre-allocate
 * buf, but don't bother zero-ing it out.
 * Does not return.
 */
void test_chain(uint8_t* buf, int nbleds, SPI_HandleTypeDef* dev, int brightness);

#endif

#endif
