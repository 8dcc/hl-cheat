
#include <stdio.h>
#include <dlfcn.h>

#include "include/main.h"
#include "include/sdk.h"
#include "include/globals.h"
#include "include/cvars.h"
#include "include/hooks.h"

static bool loaded = false;

/*
 * We need:
 *   __attribute__((constructor))
 * To indicate that this function will be the entry point once injected.
 */
__attribute__((constructor)) void load(void) {
    printf("hl-cheat injected!\n");

    /* Initialize globals/interfaces */
    if (!globals_init()) {
        fprintf(stderr, "hl-cheat: load: error loading globals, aborting\n");
        self_unload();
        return;
    }

    /* Create cvars for settings */
    if (!cvars_init()) {
        fprintf(stderr, "hl-cheat: load: error creating cvars, aborting\n");
        self_unload();
        return;
    }

    /* Hook functions */
    if (!hooks_init()) {
        fprintf(stderr, "hl-cheat: load: error hooking functions, aborting\n");
        self_unload();
        return;
    }

    i_engine->pfnClientCmd("echo \"hl-cheat loaded successfully!\"");

    loaded = true;
}

__attribute__((destructor)) void unload() {
    if (loaded) {
        /* TODO: Remove our cvars */

        globals_restore();
    }

    printf("hl-cheat unloaded.\n\n");
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
