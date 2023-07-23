
#include <stdio.h>
#include <math.h>

#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "../include/util.h"

static void autostrafe_legit(usercmd_t* cmd) {
    /* Get mouse delta */
    int dx = 0, dy = 0;
    i_engine->pfnVguiWrap2_GetMouseDelta(&dx, &dy);

    if (dx < 0)
        cmd->sidemove = -450.0f;
    else if (dx > 0)
        cmd->sidemove = 450.0f;
}

void bhop(usercmd_t* cmd) {
    if (!CVAR_ON(bhop) || i_pmove->movetype != MOVETYPE_WALK)
        return;

    static bool was_in_air = false;

    /* 2 frames in air, release jump */
    if (was_in_air && !(i_pmove->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;

    was_in_air = (i_pmove->flags & FL_ONGROUND) == 0;

    /* Autostrafe if enabled. Check if we are in the air. */
    if (was_in_air) {
        switch ((int)cv_autostrafe->value) {
            case 1:
                autostrafe_legit(cmd);
                break;
            case 0:
            default:
                break;
        }
    }
}

/*
 * Unfortunately I don't know shit about math, so this is pasted from:
 *   https://github.com/deboogerxyz/ahc/blob/0492646e28dd7234a8cd431d37b152dc18a21b04/ahc.c#L377
 */
void correct_movement(usercmd_t* cmd, vec3_t old_angles) {
    float old_y = old_angles[1] + (old_angles[1] < 0 ? 360 : 0);
    float new_y = cmd->viewangles[1] + (cmd->viewangles[1] < 0 ? 360 : 0);
    float delta = (new_y < old_y) ? fabsf(new_y - old_y)
                                  : 360 - fabsf(new_y - old_y);

    delta = 360 - delta;

    float forward = cmd->forwardmove;
    float side    = cmd->sidemove;

    cmd->forwardmove =
      cos(DEG2RAD(delta)) * forward + cos(DEG2RAD(delta + 90)) * side;
    cmd->sidemove =
      sin(DEG2RAD(delta)) * forward + sin(DEG2RAD(delta + 90)) * side;
}
