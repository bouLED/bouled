#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "sk9822strip.h"

#ifdef SIMULATION
#include <stdint.h>
#else
#include "cmsis_os.h"
#endif

#include "controller_math.h"

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

void compute_led_colors(struct frame *triangles[], quaternion_t rotation);

#endif
