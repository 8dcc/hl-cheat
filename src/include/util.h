#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <math.h>
#include "sdk.h"
#include "globals.h"

/* Vector 2 for 2d points */
typedef float vec2_t[2];

/* RGB color */
typedef struct {
    uint8_t r, g, b;
} rgb_t;

#define DEG2RAD(n) ((n)*M_PI / 180.0)
#define CLAMP(val, min, max) \
    (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#define gl_drawline_points(p0, p1, w, col) \
    gl_drawline(p0[0], p0[1], p1[0], p1[1], w, col);

/*----------------------------------------------------------------------------*/

cl_entity_t* get_player(int ent_idx);
bool is_alive(cl_entity_t* ent);
bool valid_player(cl_entity_t* ent);
char* get_name(int ent_idx);
vec3_t vec3(float x, float y, float z);
bool vec_is_zero(vec3_t v);
float vec_len2d(vec3_t v);
void vec_clamp(vec3_t v);
float angle_delta_rad(float a, float b);
bool world_to_screen(vec3_t vec, vec2_t screen);
void gl_drawbox(int x, int y, int w, int h, rgb_t c);
void gl_drawline(int x0, int y0, int x1, int y1, float w, rgb_t col);
bool unprotect_addr(void* ptr);

#endif /* UTIL_H_ */
