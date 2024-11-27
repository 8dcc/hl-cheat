
#include <math.h>

#include "include/sdk.h"
#include "include/util.h"
#include "include/mathutil.h"

vec3_t vec3(float x, float y, float z) {
    vec3_t ret;

    ret.x = x;
    ret.y = y;
    ret.z = z;

    return ret;
}

vec3_t vec_add(vec3_t a, vec3_t b) {
    vec3_t ret;

    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;

    return ret;
}

vec3_t vec_sub(vec3_t a, vec3_t b) {
    vec3_t ret;

    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;

    return ret;
}

bool vec_is_zero(vec3_t v) {
    return v.x == 0.0f && v.y == 0.0f && v.z == 0.0f;
}

float vec_len2d(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

void ang_clamp(vec3_t* v) {
    v->x = CLAMP(v->x, -89.0f, 89.0f);
    v->y = CLAMP(remainderf(v->y, 360.0f), -180.0f, 180.0f);
    v->z = CLAMP(v->z, -50.0f, 50.0f);
}

void vec_norm(vec3_t* v) {
    v->x = isfinite(v->x) ? remainderf(v->x, 360.f) : 0.f;
    v->y = isfinite(v->y) ? remainderf(v->y, 360.f) : 0.f;
    v->z = 0.0f;
}

float angle_delta_rad(float a, float b) {
    float delta = isfinite(a - b) ? remainder(a - b, 360) : 0;

    if (a > b && delta >= M_PI)
        delta -= M_PI * 2;
    else if (delta <= -M_PI)
        delta += M_PI * 2;

    return delta;
}

vec3_t vec_to_ang(vec3_t v) {
    vec3_t ret;

    ret.x = RAD2DEG(atan2(-v.z, hypot(v.x, v.y)));
    ret.y = RAD2DEG(atan2(v.y, v.x));
    ret.z = 0.0f;

    return ret;
}

vec3_t matrix_3x4_origin(matrix_3x4 m) {
    vec3_t ret;

    ret.x = m[0][3];
    ret.y = m[1][3];
    ret.z = m[2][3];

    return ret;
}

/*----------------------------------------------------------------------------*/

bool world_to_screen(vec3_t vec, vec2_t screen) {
    if (vec_is_zero(vec))
        return false;

    int engine_w2s = i_engine->pTriAPI->WorldToScreen(vec, screen);
    if (engine_w2s)
        return false;

    SCREENINFO scr_inf;
    scr_inf.iSize = sizeof(SCREENINFO);
    i_engine->pfnGetScreenInfo(&scr_inf);

    /* If within bounds, transform to screen scale */
    if (screen[0] < 1 && screen[1] < 1 && screen[0] > -1 && screen[1] > -1) {
        screen[0] = screen[0] * (scr_inf.iWidth / 2) + (scr_inf.iWidth / 2);
        screen[1] = -screen[1] * (scr_inf.iHeight / 2) + (scr_inf.iHeight / 2);

        return true;
    }

    return false;
}
