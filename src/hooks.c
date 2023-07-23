
#include "include/hooks.h"
#include "include/sdk.h"
#include "include/globals.h"

/* bhop(), esp(), etc. */
#include "features/features.h"

DECL_HOOK(CL_CreateMove);
DECL_HOOK(HUD_Redraw);

bool hooks_init(void) {
    HOOK(i_client, CL_CreateMove);
    HOOK(i_client, HUD_Redraw);

    return true;
}

void h_CL_CreateMove(float frametime, usercmd_t* cmd, int active) {
    ORIGINAL(CL_CreateMove, frametime, cmd, active);

    vec3_t old_angles = cmd->viewangles;

    /* Declared in globals.c */
    localplayer = i_engine->GetLocalPlayer();

    bhop(cmd);

    correct_movement(cmd, old_angles);
}

int h_HUD_Redraw(float time, int intermission) {
    int ret = ORIGINAL(HUD_Redraw, time, intermission);

    /* Watermark */
    i_engine->pfnDrawSetTextColor(1, 1, 1);
    i_engine->pfnDrawConsoleString(5, 5, "8dcc/hl-cheat");

#ifdef DEBUG
    char pos[100];
    sprintf(pos, "x: %f, y: %f, z: %f", localplayer->origin[0],
            localplayer->origin[1], localplayer->origin[2]);

    i_engine->pfnDrawSetTextColor(1, 1, 1);
    i_engine->pfnDrawConsoleString(5, 20, pos);
#endif

    esp();

    return ret;
}
