
#ifndef MATHUTIL_H_
#define MATHUTIL_H_ 1

#include "sdk.h"

/* Vector 2 for 2d points */
typedef float vec2_t[2];

/*----------------------------------------------------------------------------*/

#define DEG2RAD(n) ((n)*M_PI / 180.0f)
#define RAD2DEG(n) ((n)*180.0f / M_PI)
#define CLAMP(val, min, max)                                                   \
    (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

/* Use indexes so it works for float[] as well as vec3_t */
#define vec_copy(dst, src)                                                     \
    do {                                                                       \
        dst[0] = src[0];                                                       \
        dst[1] = src[1];                                                       \
        dst[2] = src[2];                                                       \
    } while (0)

/*----------------------------------------------------------------------------*/

vec3_t vec3(float x, float y, float z);
vec3_t vec_add(vec3_t a, vec3_t b);
vec3_t vec_sub(vec3_t a, vec3_t b);
bool vec_is_zero(vec3_t v);
float vec_len2d(vec3_t v);
void ang_clamp(vec3_t* v);
void vec_norm(vec3_t* v);
float angle_delta_rad(float a, float b);
vec3_t vec_to_ang(vec3_t v);
vec3_t matrix_3x4_origin(matrix_3x4 m);
bool world_to_screen(vec3_t vec, vec2_t screen);

#endif /* MATHUTIL_H_ */
