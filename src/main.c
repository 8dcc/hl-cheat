
#include <stdio.h>
#include <dlfcn.h>

#include "include/main.h"
#include "include/globals.h"
#include "include/sdk.h"

/*
 * We need:
 *   __attribute__((constructor))
 * To indicate that this function will be the entry point once injected.
 */
__attribute__((constructor)) void load(void) {
    printf("hl-cheat loaded!\n");

    if (!globals_init()) {
        printf("hl-cheats: load: error loading globals, aborting\n");
        self_unload();
        return;
    }

    printf("gp_engine: %p\n"
           "gp_client: %p\n",
           gp_engine, gp_client);

    gp_engine->pfnClientCmd("echo \"Hello from the game's console!\"");
}

void self_unload(void) {
    /*
     * RTLD_LAZY: If the symbol is never referenced, then it is never resolved.
     * RTLD_NOLOAD: Don't load the shared object.
     */
    void* self = dlopen("libhlcheat.so", RTLD_LAZY | RTLD_NOLOAD);

    /* Close the call we just made to dlopen() */
    dlclose(self);

    /* Close the call our injector made */
    dlclose(self);
}
