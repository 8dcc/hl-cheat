
#include "include/cvars.h"
#include "include/sdk.h"
#include "include/globals.h"

DECL_CVAR(bhop);
DECL_CVAR(esp);

bool cvars_init(void) {
    cv_bhop = REGISTER_CVAR("bhop", "1");
    cv_esp  = REGISTER_CVAR("esp", "1");

    return true;
}
