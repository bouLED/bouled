#ifndef CONTROLLER_MATH_H
#define CONTROLLER_MATH_H

#define PI 3.14159265359f

#ifdef SIMULATION
#include <cmath>
#else
#include <math.h>
//#define atan2(x, y) (0*x*y) //TODO: LUT
#endif

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct {
	float x;
	float y;
	float z;
	float t;
} vec4_t;

typedef struct {
	vec4_t c1;
	vec4_t c2;
	vec4_t c3;
	vec4_t c4;
} mat4_t;

typedef struct {
	float w;
	float x;
	float y;
	float z;
} quaternion_t;

mat4_t rotate_mat4(mat4_t m, quaternion_t q);
vec3_t mul_mat4_vec2(mat4_t m, vec2_t v);
vec3_t sub_vec3(vec3_t v1, vec3_t v2);
vec3_t add_vec3(vec3_t v1, vec3_t v2);
vec3_t mul_vec3(float x, vec3_t v);
mat4_t quat_to_mat4(quaternion_t q);

#endif
