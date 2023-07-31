
#include <math.h>

#include "features.h"
#include "../include/sdk.h"
#include "../include/cvars.h"
#include "../include/util.h"

/* Game units to add to the entity origin to get the head */
#define HEAD_OFFSET 25.f

/* Scale factor for aim punch */
#define AIM_PUNCH_MULT 2

static bool is_visible(vec3_t start, vec3_t end) {
    /* Syntax: PM_TraceLine(start, end, flags, usehulll, ignore_pe); */
    pmtrace_t* tr =
      i_engine->PM_TraceLine(start, end, PM_TRACELINE_PHYSENTSONLY, 2, -1);

    /* We didn't hit a valid entity */
    if (tr->ent <= 0)
        return false;

    /* Get entity index from physents, check if we can't get a valid player */
    const int ent_idx = i_pmove->physents[tr->ent].info;
    if (!get_player(ent_idx))
        return false;

    /* We hit a valid player */
    return true;
}

static vec3_t get_closest_delta(vec3_t viewangles) {
    /* Compensate aim punch. We get g_punchAngles from CalcRefdef hook */
    viewangles.x += g_punchAngles.x * AIM_PUNCH_MULT;
    viewangles.y += g_punchAngles.y * AIM_PUNCH_MULT;
    viewangles.z += g_punchAngles.z * AIM_PUNCH_MULT;

    vec3_t view_height;
    i_engine->pEventAPI->EV_LocalPlayerViewheight(view_height);
    vec3_t local_eyes = vec_add(localplayer->origin, view_height);

    /* These 2 vars are used to store the best target across iterations.
     * NOTE: The default value of best_fov will be the aimbot fov */
    float best_fov    = cv_aimbot->value;
    vec3_t best_delta = { 0, 0, 0 };

    for (int i = 1; i <= i_engine->GetMaxClients(); i++) {
        cl_entity_t* ent = get_player(i);

        if (!is_alive(ent) || is_friend(ent))
            continue;

        /* TODO: Get bones origin instead of calculating from ent origin */
        vec3_t head_pos = ent->origin;
        if (ent->curstate.usehull != 1) /* Get head if not crouched */
            head_pos.z += HEAD_OFFSET;

        if (!is_visible(local_eyes, head_pos)) /* We can't see player */
            continue;

        const vec3_t enemy_angle = vec_to_ang(vec_sub(head_pos, local_eyes));
        const vec3_t delta       = vec_sub(enemy_angle, viewangles);
        vec_norm(delta);

        float fov = hypotf(delta.x, delta.y);
        if (fov > 360.0f)
            fov = remainderf(fov, 360.0f);
        if (fov > 180.0f)
            fov = 360.0f - fov;

        if (fov < best_fov) {
            best_fov = fov;
            vec_copy(best_delta, delta);
        }
    }

    return best_delta;
}

void aimbot(usercmd_t* cmd) {
    if (!CVAR_ON(aimbot) || !(cmd->buttons & IN_ATTACK))
        return;

    /* Calculate delta with the engine viewangles, not with the cmd ones */
    vec3_t engine_viewangles;
    i_engine->GetViewAngles(engine_viewangles);

    /* TODO: Add setting for lowest health */
    vec3_t best_delta = get_closest_delta(engine_viewangles);
    if (!vec_is_zero(best_delta)) {
        /* NOTE: We can divide the best delta here to add smoothing */

        engine_viewangles.x += best_delta.x;
        engine_viewangles.y += best_delta.y;
        engine_viewangles.z += best_delta.z;
    }

    vec_copy(cmd->viewangles, engine_viewangles);

    /* NOTE: Uncomment to disable silent aim */
    /* i_engine->SetViewAngles(engine_viewangles); */
}
