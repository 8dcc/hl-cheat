
#include <stdio.h>

#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include "../include/cvars.h"

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
