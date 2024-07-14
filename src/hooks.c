
#include "include/hooks.h"
#include "include/sdk.h"
#include "include/globals.h"
#include "include/util.h"
#include "include/cvars.h"
#include "include/detour.h"    /* 8dcc/detour-lib */
#include "features/features.h" /* bhop(), esp(), etc. */

/* Normal VMT hooks */
DECL_HOOK(CL_CreateMove);
DECL_HOOK(HUD_Redraw);
DECL_HOOK(StudioRenderModel);
DECL_HOOK(CalcRefdef);
DECL_HOOK(HUD_PostRunCmd);

/* OpenGL hooks */
DECL_HOOK(glColor4f);

/* Detour hooks */
static detour_data_t detour_data_clmove;
DECL_DETOUR_TYPE(void, clmove_type);
DECL_HOOK(CL_Move);

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    /* VMT hooking */
    HOOK(i_client, CL_CreateMove);
    HOOK(i_client, HUD_Redraw);
    HOOK(i_studiomodelrenderer, StudioRenderModel);
    HOOK(i_client, CalcRefdef);
    HOOK(i_client, HUD_PostRunCmd);

    /* OpenGL hooks */
    GL_HOOK(glColor4f);

    /* Detour hooks */
    void* clmove_ptr = dlsym(hw, "CL_Move");
    if (!clmove_ptr)
        return false;

    /* Initialize detour_data_clmove struct for detour, and add the hook */
    detour_init(&detour_data_clmove, clmove_ptr, (void*)h_CL_Move);
    detour_add(&detour_data_clmove);

    return true;
}

void hooks_restore(void) {
    detour_del(&detour_data_clmove);
}

/*----------------------------------------------------------------------------*/

void h_CL_CreateMove(float frametime, usercmd_t* cmd, int active) {
    ORIGINAL(CL_CreateMove, frametime, cmd, active);

    vec3_t old_angles = cmd->viewangles;

    /* Declared in globals.c */
    localplayer = i_engine->GetLocalPlayer();

    bhop(cmd);
    aimbot(cmd);
    bullet_tracers(cmd);

    correct_movement(cmd, old_angles);
    ang_clamp(&cmd->viewangles);
}

/*----------------------------------------------------------------------------*/

int h_HUD_Redraw(float time, int intermission) {
    int ret = ORIGINAL(HUD_Redraw, time, intermission);

    /* Watermark */
    engine_draw_text(5, 5, "8dcc/hl-cheat", (rgb_t){ 255, 255, 255 });

    esp();
    custom_crosshair();

    return ret;
}

/*----------------------------------------------------------------------------*/

void h_StudioRenderModel(void* this_ptr) {
    if (!chams(this_ptr))
        ORIGINAL(StudioRenderModel, this_ptr);
}

/*----------------------------------------------------------------------------*/

void h_CalcRefdef(ref_params_t* params) {
    /* Store punch angles for CreateMove */
    vec_copy(g_punchAngles, params->punchangle);

    ORIGINAL(CalcRefdef, params);
}

/*----------------------------------------------------------------------------*/

void h_HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to,
                      struct usercmd_s* cmd, int runfuncs, double time,
                      unsigned int random_seed) {
    ORIGINAL(HUD_PostRunCmd, from, to, cmd, runfuncs, time, random_seed);

    /* Store attack information to check if we can shoot */
    if (runfuncs) {
        g_flNextAttack = to->client.m_flNextAttack;
        g_flNextPrimaryAttack =
          to->weapondata[to->client.m_iId].m_flNextPrimaryAttack;
        g_iClip = to->weapondata[to->client.m_iId].m_iClip;
    }
}

/*----------------------------------------------------------------------------*/

void h_glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    /* This visible_mode variable is changed inside the chams() function, which
     * is called from the StudioRenderModel hook.
     *
     * Depending on the type of entity we are trying to render from there, and
     * depending on its visibility, we change this visible_mode variable. */
    switch (visible_mode) {
        case ENEMY_VISIBLE:
            r = 0.40f;
            g = 0.73f;
            b = 0.41f;
            break;
        case ENEMY_NOT_VISIBLE:
            r = 0.90f;
            g = 0.07f;
            b = 0.27f;
            break;
        case FRIEND_VISIBLE:
            r = 0.16f;
            g = 0.71f;
            b = 0.96f;
            break;
        case FRIEND_NOT_VISIBLE:
            r = 0.10f;
            g = 0.20f;
            b = 0.70f;
            break;
        case HANDS:
            /* Multiply by original func parameters for non-flat chams.
             * Credits: @oxiKKK */
            r *= 0.94f;
            g *= 0.66f;
            b *= 0.94f;
            break;
        default:
        case NONE:
            break;
    }

    ORIGINAL(glColor4f, r, g, b, a);
}

/*----------------------------------------------------------------------------*/

void h_CL_Move() {
    if (cv_clmove->value != 0) {
        for (int i = 0; i < (int)cv_clmove->value; i++)
            CALL_ORIGINAL(detour_data_clmove, clmove_type);
    }

    CALL_ORIGINAL(detour_data_clmove, clmove_type);
}
