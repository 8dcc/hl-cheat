
#ifndef UTIL_H_
#define UTIL_H_ 1

#include <stdint.h>
#include <math.h>

#include "sdk.h"
#include "globals.h"

typedef struct {
    uint8_t r, g, b;
} rgb_t;

/*----------------------------------------------------------------------------*/

#define ERR(...)                                                               \
    do {                                                                       \
        fprintf(stderr, "hl-cheat: %s: ", __func__);                           \
        fprintf(stderr, __VA_ARGS__);                                          \
        fputc('\n', stderr);                                                   \
    } while (0)

#define gl_drawline_points(p0, p1, w, col)                                     \
    gl_drawline(p0[0], p0[1], p1[0], p1[1], w, col);

/*----------------------------------------------------------------------------*/

game_id get_cur_game(void);

void engine_draw_text(int x, int y, char* s, rgb_t c);
void draw_tracer(vec3_t start, vec3_t end, rgb_t c, float a, float w, float t);

void gl_drawbox(int x, int y, int w, int h, rgb_t c);
void gl_drawline(int x0, int y0, int x1, int y1, float w, rgb_t col);

bool protect_addr(void* ptr, int new_flags);

#endif /* UTIL_H_ */
