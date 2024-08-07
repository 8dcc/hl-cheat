
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include <dlfcn.h>    /* dlsym */
#include <link.h>     /* link_map */
#include <unistd.h>   /* getpagesize */
#include <sys/mman.h> /* mprotect */

#include "include/util.h"
#include "include/sdk.h"
#include "include/globals.h"

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

bool is_friend(cl_entity_t* ent) {
    if (!ent)
        return false;

    /* Check the current game because this method only works for some games */
    switch (this_game_id) {
        case TF: {
            extra_player_info_t* info = (extra_player_info_t*)player_extra_info;

            return info[ent->index].teamnumber ==
                   info[localplayer->index].teamnumber;
        }
        case CS: {
            extra_player_info_cs_t* info =
              (extra_player_info_cs_t*)player_extra_info;

            return info[ent->index].teamnumber ==
                   info[localplayer->index].teamnumber;
        }
        case DOD: {
            extra_player_info_dod_t* info =
              (extra_player_info_dod_t*)player_extra_info;

            return info[ent->index].teamnumber ==
                   info[localplayer->index].teamnumber;
        }
        case HL:
        default:
            return false;
    }
}

bool can_shoot(void) {
    return g_iClip > 0 && g_flNextAttack <= 0.0f &&
           g_flNextPrimaryAttack <= 0.0f;
}

char* get_name(int ent_idx) {
    hud_player_info_t info;
    i_engine->pfnGetPlayerInfo(ent_idx, &info);

    return info.name;
}

game_id get_cur_game(void) {
    typedef void (*COM_ParseDirectoryFromCmd_t)(const char*, char*, int,
                                                const char*);
    COM_ParseDirectoryFromCmd_t COM_ParseDirectoryFromCmd =
      (COM_ParseDirectoryFromCmd_t)dlsym(hw, "COM_ParseDirectoryFromCmd");

    char game[FILENAME_MAX];
    COM_ParseDirectoryFromCmd("-game", game, sizeof(game), "valve");

    /* Get the current game we are playing */
    if (game[0] == 'c' && game[1] == 's') /* cstrike */
        return CS;
    else if (*game == 'd') /* dod */
        return DOD;
    else if (*game == 't') /* tfc */
        return TF;
    else
        return HL;
}

/*----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------*/

void engine_draw_text(int x, int y, char* s, rgb_t c) {
    /* Convert to 0..1 range */
    float r = c.r / 255.0f;
    float g = c.g / 255.0f;
    float b = c.b / 255.0f;

    i_engine->pfnDrawSetTextColor(r, g, b);
    i_engine->pfnDrawConsoleString(x, y, s);
}

void draw_tracer(vec3_t start, vec3_t end, rgb_t c, float a, float w,
                 float time) {
    static const char* MDL_STR = "sprites/laserbeam.spr";
    static int beam_idx = i_engine->pEventAPI->EV_FindModelIndex(MDL_STR);

    float r = c.r / 255.f;
    float g = c.g / 255.f;
    float b = c.b / 255.f;

    i_engine->pEfxAPI->R_BeamPoints(start, end, beam_idx, time, w, 0, a, 0, 0,
                                    0, r, g, b);
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
    gl_drawline(x, y, x + w, y, lw, c);
    gl_drawline(x, y, x, y + h, lw, c);
    gl_drawline(x + w, y, x + w, y + h, lw, c);
    gl_drawline(x, y + h, x + w, y + h, lw, c);
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

/*----------------------------------------------------------------------------*/

void* find_sig(const char* module, const byte* pattern) {
    struct our_link_map {
        /* Base from link.h */
        ElfW(Addr) l_addr;
        const char* l_name;
        ElfW(Dyn) * l_ld;
        struct our_link_map* l_next;
        struct our_link_map* l_prev;

        /* Added */
        struct our_link_map* real;
        long int ns;
        struct libname_list* moduleName;
        ElfW(Dyn) * info[DT_NUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM +
                         DT_ADDRNUM];
        const ElfW(Phdr) * phdr;
    };

    /* TODO: Remove unnecesary cast after porting C++ SDK to C. */
    struct our_link_map* link =
      (struct our_link_map*)dlopen(module, RTLD_NOLOAD | RTLD_NOW);

    if (!link) {
        ERR("Can't open module \"%s\"", module);
        return NULL;
    }

    byte* start = (byte*)link->l_addr;
    byte* end   = start + link->phdr[0].p_memsz;

    dlclose(link);

    const byte* memPos = start;
    const byte* patPos = pattern;

    /* Iterate memory area until *patPos is '\0' (we found pattern).
     * If we start a pattern match, keep checking all pattern positions until we
     * are done or until mismatch. If we find mismatch, reset pattern position
     * and continue checking at the memory location where we started +1 */
    while (memPos < end && *patPos != '\0') {
        if (*memPos == *patPos || *patPos == '?') {
            memPos++;
            patPos++;
        } else {
            start++;
            memPos = start;
            patPos = pattern;
        }
    }

    /* We reached end of pattern, we found it */
    if (*patPos == '\0')
        return start;

    return NULL;
}

#define PAGE_MASK          (~(PAGE_SZ - 1))
#define PAGE_ALIGN(x)      ((x + PAGE_SZ - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (PAGE_ALIGN(x) - PAGE_SZ)

bool protect_addr(void* ptr, int new_flags) {
    int PAGE_SZ = getpagesize();
    void* p     = (void*)PAGE_ALIGN_DOWN((uint32_t)ptr);

    if (mprotect(p, PAGE_SZ, new_flags) == -1) {
        ERR("Error protecting %p", ptr);
        return false;
    }

    return true;
}
