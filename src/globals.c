
#include <stdio.h>
#include <dlfcn.h>

#include "include/globals.h"
#include "include/sdk.h"

void* hw;
DECL_INTF(cl_enginefunc_t, engine);
DECL_INTF(cl_clientfunc_t, client);
DECL_INTF(playermove_t, pmove);

bool globals_init(void) {
    /*
     * Get handler for hw.so
     *  RTLD_LAZY: If the symbol is never referenced, then it is never resolved.
     *  RTLD_NOLOAD: Don't load the shared object.
     */
    hw = dlopen("hw.so", RTLD_LAZY | RTLD_NOLOAD);

    /* Make sure it's a valid handler */
    if (!hw) {
        printf("hl-cheat: globals_init: can't open hw.so\n");
        return false;
    }

    /* Get symbol addresses using dlsym and the handler we just opened */
    i_engine = (cl_enginefunc_t*)dlsym(hw, "cl_enginefuncs");
    i_client = (cl_clientfunc_t*)dlsym(hw, "cl_funcs");
    i_pmove  = *(playermove_t**)dlsym(hw, "pmove");

    if (!i_engine || !i_client || !i_pmove) {
        printf("hl-cheat: globals_init: could't load some symbols.\n");
        return false;
    }

    return true;
}
