
#include "include/esp.h"
#include "../include/globals.h"
#include "../include/cvars.h"

/* TODO: Move all these to utils.c */
/* TODO: Replace src/features/include/X.h -> src/features/features.h */
static bool is_alive(cl_entity_t* ent) {
    return ent && ent->curstate.movetype != 6 && ent->curstate.movetype != 0;
}

static bool valid_client(cl_entity_t* ent) {
    return is_alive(ent) && ent->index != localplayer->index;
}

static bool vec_is_zero(vec3_t v) {
    return v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f;
}

static char* get_name(int ent_idx) {
    hud_player_info_t info;
    i_engine->pfnGetPlayerInfo(ent_idx, &info);

    return info.name;
}

typedef float vec2_t[2];
static bool world_to_screen(vec3_t vec, vec2_t screen) {
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

void esp(void) {
    if (!CVAR_ON(esp))
        return;

    /* Iterate all clients */
    for (int i = 1; i <= i_engine->GetMaxClients(); i++) {
        cl_entity_t* ent = i_engine->GetEntityByIndex(i);

        if (!valid_client(ent) || vec_is_zero(ent->origin))
            continue;

        float screen_point[2];
        if (world_to_screen(ent->origin, screen_point)) {
            i_engine->pfnDrawSetTextColor(1, 1, 1);
            i_engine->pfnDrawConsoleString(screen_point[0], screen_point[1],
                                           get_name(ent->index));
        }
    }
}
