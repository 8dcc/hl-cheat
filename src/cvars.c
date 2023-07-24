
#include "include/cvars.h"
#include "include/sdk.h"
#include "include/globals.h"

DECL_CVAR(bhop);
DECL_CVAR(autostrafe);
DECL_CVAR(esp);
DECL_CVAR(chams);

bool cvars_init(void) {
    cv_bhop       = REGISTER_CVAR("bhop", "1");
    cv_autostrafe = REGISTER_CVAR("autostrafe", "0");
    cv_esp        = REGISTER_CVAR("esp", "1");
    cv_chams      = REGISTER_CVAR("chams", "1");

    return true;
}
