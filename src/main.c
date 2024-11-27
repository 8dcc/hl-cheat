
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
    printf("hl-cheat: Injected.\n");

    /* Initialize globals/interfaces */
    if (!globals_init()) {
        ERR("Error loading globals, aborting");
        self_unload();
        return;
    }

    /* Create cvars for settings */
    if (!cvars_init()) {
        ERR("Error creating cvars, aborting");
        self_unload();
        return;
    }

    /* Hook functions */
    if (!hooks_init()) {
        ERR("Error hooking functions, aborting");
        self_unload();
        return;
    }

    /* Get game version after injecting */
    this_game_id = get_cur_game();

    i_engine->pfnClientCmd("echo \"hl-cheat loaded successfully!\"");

    loaded = true;
}

__attribute__((destructor)) /* Entry point when unloaded */
void unload(void) {
    if (loaded) {
        /* TODO: Remove our cvars */

        globals_restore();
        hooks_restore();

        /* Manually restore OpenGL hooks here */
        GL_UNHOOK(glColor4f);

        /* Close hw.so handler */
        dlclose(hw);
    }

    printf("hl-cheat: Unloaded.\n\n");
}

void self_unload(void) {
    /*
     * RTLD_LAZY: If the symbol is never referenced, then it is never resolved.
     * RTLD_NOLOAD: Don't load the shared object.
     */
    void* self = dlopen("libhlcheat.so", RTLD_LAZY | RTLD_NOLOAD);

    /* Close the call we just made to `dlopen' */
    dlclose(self);

    /* Close the call that our injector made */
    dlclose(self);
}
