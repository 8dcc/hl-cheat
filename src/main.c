
#include <stdio.h>
#include <dlfcn.h>

#include "include/main.h"
#include "include/sdk.h"
#include "include/globals.h"
#include "include/cvars.h"
#include "include/hooks.h"
#include "include/util.h"

static bool loaded = false;

__attribute__((constructor)) /* Entry point when injected */
void load(void) {
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

    /* Get game version after injecting */
    this_game_id = get_cur_game();

    i_engine->pfnClientCmd("echo \"hl-cheat loaded successfully!\"");

    loaded = true;
}

__attribute__((destructor)) /* Entry point when unloaded */
void unload() {
    if (loaded) {
        /* TODO: Remove our cvars */

        globals_restore();
        hooks_restore();

        GL_UNHOOK(glColor4f); /* Manually restore OpenGL hooks here */
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
