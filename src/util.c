

#include "include/util.h"
#include "include/sdk.h"
#include "include/globals.h"

#include <math.h>
#include <GL/gl.h>
#include <unistd.h>   /* getpagesize */
#include <sys/mman.h> /* mprotect */

cl_entity_t* get_player(int ent_idx) {
    if (ent_idx < 0 || ent_idx > 32)
        return NULL;

    cl_entity_t* ent = i_engine->GetEntityByIndex(ent_idx);

    if (!valid_player(ent))
        return NULL;

    return ent;
}

bool is_alive(cl_entity_t* ent) {
    return ent && ent->curstate.movetype != 6 && ent->curstate.movetype != 0;
}

bool valid_player(cl_entity_t* ent) {
    return ent && ent->player && ent->index != localplayer->index &&
           ent->curstate.messagenum >= localplayer->curstate.messagenum;
}

char* get_name(int ent_idx) {
    hud_player_info_t info;
    i_engine->pfnGetPlayerInfo(ent_idx, &info);

    return info.name;
}

vec3_t vec3(float x, float y, float z) {
    vec3_t ret;

    ret[0] = x;
    ret[1] = y;
    ret[2] = z;

    return ret;
}

bool vec_is_zero(vec3_t v) {
    return v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f;
}

float vec_len2d(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

void vec_clamp(vec3_t v) {
    v[0] = CLAMP(v[0], -89.0f, 89.0f);
    v[1] = CLAMP(remainderf(v[1], 360.0f), -180.0f, 180.0f); /* v.y % 360 */
    v[2] = CLAMP(v[2], -50.0f, 50.0f);
}

float angle_delta_rad(float a, float b) {
    float delta = isfinite(a - b) ? remainder(a - b, 360) : 0;

    if (a > b && delta >= M_PI)
        delta -= M_PI * 2;
    else if (delta <= -M_PI)
        delta += M_PI * 2;

    return delta;
}

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

void gl_drawbox(int x, int y, int w, int h, rgb_t c) {
    /* Line width */
    const int lw = 1;

    /*
     *     1
     *   +----+
     * 2 |    | 3
     *   |    |
     *   +----+
     *     4
     */
    gl_drawline(x, y, x + w, y, lw, c);         /* 1 */
    gl_drawline(x, y, x, y + h, lw, c);         /* 2 */
    gl_drawline(x + w, y, x + w, y + h, lw, c); /* 3 */
    gl_drawline(x, y + h, x + w, y + h, lw, c); /* 4 */
}

void gl_drawline(int x0, int y0, int x1, int y1, float w, rgb_t col) {
    const int alpha = 255;

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(col.r, col.g, col.b, alpha); /* Set colors + alpha */
    glLineWidth(w);                         /* Set line width */
    glBegin(GL_LINES);                      /* Interpret vertices as lines */
    glVertex2i(x0, y0);                     /* Start */
    glVertex2i(x1, y1);                     /* End */
    glEnd();                                /* Stop glBegin, end line mode */
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

/*
 * Credits:
 *   https://github.com/UnkwUsr/hlhax/blob/26491984996c8389efec977ed940c5a67a0ecca4/src/utils/mem/mem.cpp
 *   Linux kernel, tools/virtio/linux/kernel.h
 */
#define PAGE_SIZE          getpagesize()
#define PAGE_MASK          (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(x)      ((x + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (PAGE_ALIGN(x) - PAGE_SIZE)

bool protect_addr(void* ptr, int new_flags) {
    void* p  = (void*)PAGE_ALIGN_DOWN((int)ptr);
    int pgsz = getpagesize();

    if (mprotect(p, pgsz, new_flags) == -1) {
        printf("hl-cheat: error protecting %p\n", ptr);
        return false;
    }

    return true;
}
