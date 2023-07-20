
#include "include/hooks.h"
#include "include/sdk.h"
#include "include/globals.h"

DECL_HOOK(CL_CreateMove);

bool hooks_init(void) {
    HOOK(i_client, CL_CreateMove);

    return true;
}

void h_CL_CreateMove(float frametime, usercmd_t* cmd, int active) {
    /* bhop(cmd); */

    ORIGINAL(CL_CreateMove, frametime, cmd, active);
}
