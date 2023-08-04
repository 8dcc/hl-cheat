
#include <stdio.h>
#include <math.h>

#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "../include/util.h"

static void autostrafe_legit(usercmd_t* cmd) {
    /* Get mouse delta */
    int dx = 0, dy = 0;
    i_engine->pfnVguiWrap2_GetMouseDelta(&dx, &dy);

    if (dx < 0)
        cmd->sidemove = -450.0f;
    else if (dx > 0)
        cmd->sidemove = 450.0f;
}

/*
 * See:
 *   https://github.com/deboogerxyz/ahc/blob/0492646e28dd7234a8cd431d37b152dc18a21b04/ahc.c#L201
 *   https://github.com/NullHooks/NullHooks/blob/535351569ca599cadd21a286d88098b6dc057a46/src/core/features/movement/bhop.cpp#L73
 */
static void autostrafe_rage(usercmd_t* cmd) {
    if (i_pmove->movetype != MOVETYPE_WALK)
        return;

    /* TODO: Get at runtime */
    const float sv_airaccelerate = 10.0f;
    const float sv_maxspeed      = 320.0f;
    const float cl_forwardspeed  = 425.0f;
    const float cl_sidespeed     = 400.0f;

    float speed = vec_len2d(i_pmove->velocity);

    /* If low speed, start forward */
    if (speed < 15 && (cmd->buttons & IN_FORWARD)) {
        cmd->forwardmove = 450.0f;
        return;
    }

    float term = sv_airaccelerate / sv_maxspeed * 100.0f / speed;
    if (term < -1 || term > 1)
        return;

    float best_delta = acosf(term);

    /* Use engine viewangles in case we do something nasty with cmd's angles */
    vec3_t viewangles;
    i_engine->GetViewAngles(viewangles);

    /* Get our desired angles and delta */
    float yaw        = DEG2RAD(viewangles.y);
    float vel_dir    = atan2f(i_pmove->velocity.y, i_pmove->velocity.x) - yaw;
    float target_ang = atan2f(-cmd->sidemove, cmd->forwardmove);
    float delta      = angle_delta_rad(vel_dir, target_ang);

    float movedir = delta < 0 ? vel_dir + best_delta : vel_dir - best_delta;

    cmd->forwardmove = cosf(movedir) * cl_forwardspeed;
    cmd->sidemove    = -sinf(movedir) * cl_sidespeed;
}

void bhop(usercmd_t* cmd) {
    if (!CVAR_ON(bhop) || i_pmove->movetype != MOVETYPE_WALK)
        return;

    static bool was_in_air = false;

    /* Used bellow to check if we should autostrafe before. Store since we might
     * change cmd->buttons (autostrafe only when user hold space) */
    bool is_jumping = cmd->buttons & IN_JUMP;

    /* 2 frames in air, release jump */
    if (was_in_air && !(i_pmove->flags & FL_ONGROUND))
        cmd->buttons &= ~IN_JUMP;

    was_in_air = (i_pmove->flags & FL_ONGROUND) == 0;

    /* Autostrafe if enabled. Check if we are in the air and holding space. */
    if (is_jumping) {
        switch ((int)cv_autostrafe->value) {
            case 1:
                autostrafe_rage(cmd);
                break;
            case 2:
                autostrafe_legit(cmd);
                break;
            case 0:
            default:
                break;
        }
    }
}

/*
 * Unfortunately I don't know shit about math, so this is pasted from:
 *   https://github.com/deboogerxyz/ahc/blob/0492646e28dd7234a8cd431d37b152dc18a21b04/ahc.c#L377
 */
void correct_movement(usercmd_t* cmd, vec3_t old_angles) {
    float old_y = old_angles.y + (old_angles.y < 0 ? 360 : 0);
    float new_y = cmd->viewangles.y + (cmd->viewangles.y < 0 ? 360 : 0);
    float delta = (new_y < old_y) ? fabsf(new_y - old_y)
                                  : 360 - fabsf(new_y - old_y);

    delta = 360 - delta;

    float forward = cmd->forwardmove;
    float side    = cmd->sidemove;

    cmd->forwardmove =
      cos(DEG2RAD(delta)) * forward + cos(DEG2RAD(delta + 90)) * side;
    cmd->sidemove =
      sin(DEG2RAD(delta)) * forward + sin(DEG2RAD(delta + 90)) * side;
}
