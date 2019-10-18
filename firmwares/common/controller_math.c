#include "controller_math.h"

#define at(m, i, j) ((&((&m.c1)[j].x))[i])

mat4_t mult_mat4(mat4_t m1, mat4_t m2);
mat4_t zero_mat4();

mat4_t rotate_mat4(mat4_t m, quaternion_t q) {
	return mult_mat4(quat_to_mat4(q), m); // we could also use a mat3 for q
}

vec3_t mul_mat4_vec2(mat4_t m, vec2_t v) {
	return (vec3_t) {m.c1.x * v.x + m.c2.x * v.y + m.c4.x,
		               m.c1.y * v.x + m.c2.y * v.y + m.c4.y,
		               m.c1.z * v.x + m.c2.z * v.y + m.c4.z};
}

vec3_t sub_vec3(vec3_t v1, vec3_t v2) {
	return (vec3_t) {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

mat4_t quat_to_mat4(quaternion_t q) {
	vec4_t c1 = {1.0f - 2.0f*q.y*q.y - 2.0f*q.z*q.z,
		           2.0f*q.x*q.y + 2.0f*q.z*q.w,
		           2.0f*q.x*q.z - 2.0f*q.y*q.w,
		           0.0f};
	vec4_t c2 = {2.0f*q.x*q.y - 2.0f*q.z*q.w,
		           1.0f - 2.0f*q.x*q.x - 2.0f*q.z*q.z,
		           2.0f*q.y*q.z + 2.0f*q.x*q.w,
		           0.0f};
	vec4_t c3 = {2.0f*q.x*q.z + 2.0f*q.y*q.w,
		           2.0f*q.y*q.z - 2.0f*q.x*q.w,
		           1.0f - 2.0f*q.x*q.x - 2.0f*q.y*q.y,
		           0.0f};
	vec4_t c4 = {0.0f,
		           0.0f,
		           0.0f,
		           1.0f};
	return (mat4_t) {c1, c2, c3, c4};
}

vec3_t add_vec3(vec3_t v1, vec3_t v2) {
	return (vec3_t) {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3_t mul_vec3(float x, vec3_t v) {
	return (vec3_t) {x*v.x, x*v.y, x*v.z};
}

mat4_t mult_mat4(mat4_t m1, mat4_t m2) {
	mat4_t result = zero_mat4();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				at(result, i, j) += at(m1, i, k) * at(m2, k, j);
	return result;
}

mat4_t zero_mat4() {
	return (mat4_t) {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};
}

