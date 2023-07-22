
#include "include/util.h"
#include "include/sdk.h"
#include "include/globals.h"

bool is_alive(cl_entity_t* ent) {
    return ent && ent->curstate.movetype != 6 && ent->curstate.movetype != 0;
}

bool valid_client(cl_entity_t* ent) {
    return is_alive(ent) && ent->index != localplayer->index;
}

char* get_name(int ent_idx) {
    hud_player_info_t info;
    i_engine->pfnGetPlayerInfo(ent_idx, &info);

    return info.name;
}

bool vec_is_zero(vec3_t v) {
    return v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f;
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
