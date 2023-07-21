
#include <stdio.h>

#include "include/movement.h"
#include "../include/sdk.h"
#include "../include/globals.h"

void bhop(usercmd_t* cmd) {
    /* TODO: Check !bhop_var */
    if (false || i_pmove->movetype != MOVETYPE_WALK)
        return;

    static bool was_in_air = false;

    /* 2 frames in air, release jump */
    if (was_in_air && !(i_pmove->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;

    was_in_air = (i_pmove->flags & FL_ONGROUND) == 0;
}
