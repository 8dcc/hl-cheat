
#include <math.h>

#include "features.h"
#include "../include/sdk.h"
#include "../include/cvars.h"
#include "../include/util.h"

/* Game units to add to the entity origin to get the head */
#define HEAD_OFFSET 0.8f

static vec3_t get_closest_delta(vec3_t viewangles) {
    vec3_t view_height;
    i_engine->pEventAPI->EV_LocalPlayerViewheight(view_height);
    vec3_t local_eyes = vec_add(localplayer->origin, view_height);
    /* TODO: Compensate aim punch */

    /* These 2 vars are used to store the best target across iterations.
     * NOTE: The default value of best_fov will be the aimbot fov */
    float best_fov    = cv_aimbot->value;
    vec3_t best_delta = { 0, 0, 0 };

    for (int i = 1; i <= i_engine->GetMaxClients(); i++) {
        cl_entity_t* ent = get_player(i);

        if (!is_alive(ent) || is_friend(ent))
            continue;

        /* TODO: Get bones origin instead of calculating from ent origin */
        const vec3_t head_pos = vec_add(ent->origin, vec3(0, 0, HEAD_OFFSET));
        const vec3_t enemy_angle = vec_to_ang(vec_sub(head_pos, local_eyes));

        const vec3_t delta = vec_sub(enemy_angle, viewangles);
        vec_norm(delta);

        float fov = hypotf(delta.x, delta.y);
        if (fov > 360.0f) {
            fov = remainderf(fov, 360.0f);
            if (fov > 180.0f)
                fov = 360.0f - fov;
        }

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