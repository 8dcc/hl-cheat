
#include "include/cvars.h"
#include "include/sdk.h"
#include "include/globals.h"

DECL_CVAR(bhop);
DECL_CVAR(autostrafe);
DECL_CVAR(aimbot);
DECL_CVAR(esp);
DECL_CVAR(chams);
DECL_CVAR(crosshair);
DECL_CVAR(clmove);

bool cvars_init(void) {
    REGISTER_CVAR(bhop, 1);
    REGISTER_CVAR(autostrafe, 0);
    REGISTER_CVAR(aimbot, 0);
    REGISTER_CVAR(esp, 3);
    REGISTER_CVAR(chams, 1);
    REGISTER_CVAR(crosshair, 0);
    REGISTER_CVAR(clmove, 0);

    return true;
}
