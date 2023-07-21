
#include "include/cvars.h"
#include "include/sdk.h"
#include "include/globals.h"

DECL_CVAR(bhop);

bool cvars_init(void) {
    cv_bhop = REGISTER_CVAR("bhop", "1");

    return true;
}
