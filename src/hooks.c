
#include "include/hooks.h"
#include "include/sdk.h"
#include "include/globals.h"
#include "include/util.h"
#include "include/detour.h" /* 8dcc/detour-lib */

/* bhop(), esp(), etc. */
#include "features/features.h"

DECL_HOOK(CL_CreateMove);
DECL_HOOK(HUD_Redraw);
DECL_HOOK(StudioRenderModel);

DECL_HOOK(glColor4f);

/* For detour hooking CL_Move */
static detour_data_t clmove_data;
DECL_DETOUR_TYPE(void, clmove);
DECL_HOOK(CL_Move);

bool hooks_init(void) {
    HOOK(i_client, CL_CreateMove);
    HOOK(i_client, HUD_Redraw);
    HOOK(i_studiomodelrenderer, StudioRenderModel);

    GL_HOOK(glColor4f);

    void* clmove_ptr = dlsym(hw, "CL_Move");
    if (!clmove_ptr)
        return false;

    /* Initialize clmove_data struct for detour, and add the hook */
    detour_init(&clmove_data, clmove_ptr, (void*)h_CL_Move);
    detour_add(&clmove_data);

    return true;
}

void hooks_restore(void) {
    detour_del(&clmove_data);
}

/*----------------------------------------------------------------------------*/

void h_CL_CreateMove(float frametime, usercmd_t* cmd, int active) {
    ORIGINAL(CL_CreateMove, frametime, cmd, active);

    vec3_t old_angles = cmd->viewangles;

    /* Declared in globals.c */
    localplayer = i_engine->GetLocalPlayer();

    bhop(cmd);

    correct_movement(cmd, old_angles);
    vec_clamp(cmd->viewangles);
}

/*----------------------------------------------------------------------------*/

int h_HUD_Redraw(float time, int intermission) {
    int ret = ORIGINAL(HUD_Redraw, time, intermission);

    /* Watermark */
    i_engine->pfnDrawSetTextColor(1, 1, 1);
    i_engine->pfnDrawConsoleString(5, 5, "8dcc/hl-cheat");

#ifdef DEBUG
    char pos[100];
    sprintf(pos, "x: %f, y: %f, z: %f", localplayer->origin[0],
            localplayer->origin[1], localplayer->origin[2]);

    i_engine->pfnDrawSetTextColor(1, 1, 1);
    i_engine->pfnDrawConsoleString(5, 20, pos);
#endif

    esp();

    return ret;
}

/*----------------------------------------------------------------------------*/

void h_StudioRenderModel(void* this_ptr) {
    if (!chams(this_ptr))
        ORIGINAL(StudioRenderModel, this_ptr);
}

/*----------------------------------------------------------------------------*/

void h_glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    /* This visible_mode variable is changed inside the chams() function, which
     * is called from the StudioRenderModel hook.
     *
     * Depending on the type of entity we are trying to render from there, and
     * depending on its visibility, we change this visible_mode variable. */
    switch (visible_mode) {
        case VISIBLE:
            r = 0.40f;
            g = 0.73f;
            b = 0.41f;
            break;
        case NOT_VISIBLE:
            r = 0.90f;
            g = 0.07f;
            b = 0.27f;
            break;
        default:
        case NONE:
            break;
    }

    ORIGINAL(glColor4f, r, g, b, a);
}

/*----------------------------------------------------------------------------*/

void h_CL_Move() {
    /* printf("Hello from CL_Move!\n"); */
    CALL_ORIGINAL(clmove_data, clmove);
}
