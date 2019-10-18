#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "sk9822strip.h"

#define NB_LEDSPERTRI 78
#define NB_TRIANGLES 20

/* Usage: triangles[triangle number][led number] */
extern struct frame* triangles[NB_TRIANGLES];

void led_strip_init(void);
void led_task(void *pv);

#endif
