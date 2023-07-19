
#include <stdio.h>
#include <dlfcn.h>

#include "include/globals.h"
#include "include/sdk.h"

void* hw;
DECLARE_SYMBOLS(cl_enginefunc_t, engine);
DECLARE_SYMBOLS(cl_clientfunc_t, client);

bool globals_init(void) {
    /*
     * Get handler for hw.so
     *  RTLD_LAZY: If the symbol is never referenced, then it is never resolved.
     *  RTLD_NOLOAD: Don't load the shared object.
     */
    hw = dlopen("hw.so", RTLD_LAZY | RTLD_NOLOAD);

    /* Make sure it's a valid handler */
    if (!hw) {
        printf("hl-cheats: globals_init: can't open hw.so\n");
        return false;
    }

    /* Get symbol addresses using dlsym and the handler we just opened */
    gp_engine = (cl_enginefunc_t*)dlsym(hw, "cl_enginefuncs");
    gp_client = (cl_clientfunc_t*)dlsym(hw, "cl_funcs");

    return true;
}
