
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h> /* PROT_* */

#include "include/globals.h"
#include "include/sdk.h"
#include "include/util.h"

#define GET_PATTERN(VAR, MODULE, SIG)           \
    void* VAR = find_sig(MODULE, SIG);          \
    if (!VAR) {                                 \
        ERR("Coundn't find pattern for " #SIG); \
        return false;                           \
    }

/*----------------------------------------------------------------------------*/

/* Handlers used globally */
void* hw = NULL;

game_id this_game_id = HL;
vec3_t g_punchAngles = { 0, 0, 0 };

/* Weapon info */
float g_flNextAttack = 0.f, g_flNextPrimaryAttack = 0.f;
int g_iClip = 0;

DECL_INTF(cl_enginefunc_t, engine);
DECL_INTF(cl_clientfunc_t, client);
DECL_INTF(playermove_t, pmove);
DECL_INTF(engine_studio_api_t, enginestudio);
DECL_INTF(StudioModelRenderer_t, studiomodelrenderer);

/* Game struct with some useful info */
game_t* game_info;

/* Array of extra_player_info's for each player */
void* player_extra_info;

/* Updated in CL_CreateMove hook */
cl_entity_t* localplayer = NULL;

/*----------------------------------------------------------------------------*/

bool globals_init(void) {
    /*
     * Get handler for hw.so
     *  RTLD_LAZY: If the symbol is never referenced, then it is never resolved.
     *  RTLD_NOLOAD: Don't load the shared object.
     */
    hw = dlopen("hw.so", RTLD_LAZY | RTLD_NOLOAD);
    if (!hw) {
        ERR("Can't open hw.so");
        return false;
    }

    void* h_client = *(void**)dlsym(hw, "hClientDLL");
    if (!h_client) {
        ERR("Can't find hClientDLL");
        return false;
    }

    /* Get symbol addresses using dlsym and the handler we just opened */
    i_engine       = (cl_enginefunc_t*)dlsym(hw, "cl_enginefuncs");
    i_client       = (cl_clientfunc_t*)dlsym(hw, "cl_funcs");
    i_pmove        = *(playermove_t**)dlsym(hw, "pmove");
    i_enginestudio = (engine_studio_api_t*)dlsym(hw, "engine_studio_api");

    const char* SMR_STR   = "g_StudioRenderer"; /* For clang-format */
    i_studiomodelrenderer = *(StudioModelRenderer_t**)dlsym(h_client, SMR_STR);

    player_extra_info = dlsym(h_client, "g_PlayerExtraInfo");

    game_info = *(game_t**)dlsym(hw, "game");

    if (!i_engine || !i_client || !i_pmove || !i_enginestudio ||
        !i_studiomodelrenderer || !game_info) {
        ERR("Couldn't load some symbols");
        return false;
    }

    if (!protect_addr(i_studiomodelrenderer, PROT_READ | PROT_WRITE)) {
        ERR("Couldn't unprotect address of SMR");
        return false;
    }

    globals_store();

    return true;
}

void globals_store(void) {
    memcpy(&o_engine, i_engine, sizeof(cl_enginefunc_t));
    memcpy(&o_client, i_client, sizeof(cl_clientfunc_t));
    memcpy(&o_enginestudio, i_enginestudio, sizeof(engine_studio_api_t));
}

void globals_restore(void) {
    memcpy(i_engine, &o_engine, sizeof(cl_enginefunc_t));
    memcpy(i_client, &o_client, sizeof(cl_clientfunc_t));
    memcpy(i_enginestudio, &o_enginestudio, sizeof(engine_studio_api_t));
}
