#include "controller.h"
#include "controller_constants.h"
#include "sk9822strip.h"

#ifdef SIMULATION
#include <cmath>
#else
#include <math.h>
#endif

/*
 * Computing a square root is not needed.
 * We could just pre-compute the distance
 * between each of the 78 LEDs to the
 * origin.
 */
float length(vec3_t v) {
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

void compute_led_colors(struct frame *triangles[], quaternion_t rotation) {
	for (int face = 0; face < 20; face++) {
		int led_color_index = 0;
		mat4_t face_position = rotate_mat4(face_positions[face], rotation);
		vec3_t led_positions_rotated[3];
		// Compute the position of the corner LEDs
		for (int i = 0; i < 3; i++)
			led_positions_rotated[i] = mul_mat4_vec2(face_position, led_positions[i]);

		// Then interpolater between the corner LEDs
		for (int row = 0; row < NUMBER_OF_ROWS; row++) {
			vec3_t first_in_row = add_vec3(led_positions_rotated[0], mul_vec3(((float)row)/(NUMBER_OF_ROWS-1), sub_vec3(led_positions_rotated[2], led_positions_rotated[0])));
			for (int i = 0; i < NUMBER_OF_ROWS - row; i++) {
#ifdef SIMULATION
				int led = i;
#else
				int led = (row%2==0)?i:(NUMBER_OF_ROWS - row - i - 1); // led ordered as a snake
#endif
				vec3_t led_position = add_vec3(first_in_row, mul_vec3(((float)led)/(NUMBER_OF_ROWS-1), sub_vec3(led_positions_rotated[1], led_positions_rotated[0])));
				led_position = mul_vec3(1.0f/length(led_position), led_position);
				int latitude = imageH - 1 - ((int) ((imageH-1) * (0.5f - led_position.y / (2.0f))));
				int longitude = imageW - 1 - (int) ((imageW-1) * (atan2(led_position.x, led_position.z) + PI) / (2.0f*PI));
				triangles[face][led_color_index].r = image[latitude*imageW+longitude].r;
				triangles[face][led_color_index].g = image[latitude*imageW+longitude].g;
				triangles[face][led_color_index].b = image[latitude*imageW+longitude].b;
				led_color_index++;
			}
		}
	}
}
