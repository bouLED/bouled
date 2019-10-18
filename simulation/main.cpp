#include "visualisation.hpp"
#include "controller.h"
#include <cstdio>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <atomic>
#include "sk9822strip.h"

#include <glm/gtc/quaternion.hpp>

#define abs(x) (((x)>0)?x:(-(x)))

#define NB_TRI_IN_CHAIN 20
#define NB_LEDS_IN_TRI 78
#define NB_LEDS (NB_TRI_IN_CHAIN*NB_LEDS_IN_TRI)

DECLLEDBUF(buf, NB_LEDS);

Visualisation *v;

std::atomic<bool> ended(false);
void graphicalThread() {
	v->start();
	ended = true;
}

void inputThread() {
	int numberOfLedsPerTriangle = v->getNumberOfLedsPerTriangle();
	struct frame* leds = stripbuf_init(buf, NB_LEDS);
	struct frame* triangles[20];
	for (int i = 0; i < 20; i++)
		triangles[i] = &leds[((i-10+20)%20)*NB_LEDS_IN_TRI];

	glm::quat q(1.0f, 0.0f, 0.0f, 0.0f);

	while (true) {
		float w, x, y, z;
		quaternion_t q2;
		v->setRotation(q);

		//setRotation(q);
		compute_led_colors(triangles, q2);
		// Set the LEDs to some colors
		v->lock.lock(); // not elegant but simple
		for (int face = 0; face < 20; face++) {
			v->setFaceColor(face, 0, 0, 0);
			int led_index = 0;
			for (int led = 0; led < numberOfLedsPerTriangle; led++) {
				struct frame led_color = triangles[face][led_index++];
				v->setLedColor(face, led, led_color.r, led_color.g, led_color.b);
			}
		}
		v->flushColors();
		v->lock.unlock();
		if (scanf("%f,%f,%f,%f", &w, &x, &y, &z) == 4) {
			q = glm::quat(w, -y, -z, x);
			q2.x = -y;
			q2.y = -z;
			q2.z = x;
			q2.w = w;
		} else {
			std::cout << "Badly formatted quaternion" << std::endl;
			while (getchar() != '\n') ; // Clear input buffer
		}
	}
}

int main(void) {
	int face_orientations[20] = {1, 0, 1, 1, 0,
	                             0, -1, 1, 1, 1,
	                             -1, 0, 0, -1, 0,
	                             -1, -1, 0, -1, 0};
	v = new Visualisation(face_orientations);
	//if (controllerInit(face_orientations, "image.ppm") != 0)
	//	return -1;
	//setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	std::thread t1(graphicalThread);
	std::thread t2(inputThread);

	int i = 0;
	while(!ended) {
		i = (i+1)%(2*255);

		usleep(10000);
	}

	t1.join();
	/* This means, roughly, not to worry about t2. Doing that avoids a
	 * SIGABRT */
	t2.detach();
	return 0;
}
