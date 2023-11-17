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

#define ERR(...)                                     \
    do {                                             \
        fprintf(stderr, "hl-cheat: %s: ", __func__); \
        fprintf(stderr, __VA_ARGS__);                \
        fputc('\n', stderr);                         \
    } while (0);

#define DEG2RAD(n) ((n)*M_PI / 180.0f)
#define RAD2DEG(n) ((n)*180.0f / M_PI)
#define CLAMP(val, min, max) \
    (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#define gl_drawline_points(p0, p1, w, col) \
    gl_drawline(p0[0], p0[1], p1[0], p1[1], w, col);

/* Use indexes so it works for float[] as well as vec3_t */
#define vec_copy(dst, src) \
    dst[0] = src[0];       \
    dst[1] = src[1];       \
    dst[2] = src[2];

/*----------------------------------------------------------------------------*/

cl_entity_t* get_player(int ent_idx);
bool is_alive(cl_entity_t* ent);
bool valid_player(cl_entity_t* ent);
bool is_friend(cl_entity_t* ent);
bool can_shoot(void);
char* get_name(int ent_idx);
game_id get_cur_game(void);
vec3_t vec3(float x, float y, float z);
vec3_t vec_add(vec3_t a, vec3_t b);
vec3_t vec_sub(vec3_t a, vec3_t b);
bool vec_is_zero(vec3_t v);
float vec_len2d(vec3_t v);
void vec_clamp(vec3_t v);
void vec_norm(vec3_t v);
float angle_delta_rad(float a, float b);
vec3_t vec_to_ang(vec3_t v);
vec3_t matrix_3x4_origin(matrix_3x4 m);
bool world_to_screen(vec3_t vec, vec2_t screen);
void engine_draw_text(int x, int y, char* s, rgb_t c);
void draw_tracer(vec3_t start, vec3_t end, rgb_t c, float a, float w, float t);
void gl_drawbox(int x, int y, int w, int h, rgb_t c);
void gl_drawline(int x0, int y0, int x1, int y1, float w, rgb_t col);
bool protect_addr(void* ptr, int new_flags);

#endif /* UTIL_H_ */
