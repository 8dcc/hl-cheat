
#include <math.h>

#include "features.h"
#include "../include/sdk.h"
#include "../include/cvars.h"
#include "../include/util.h"

enum aimbot_settings {
    OFF           = 0,
    CLOSEST       = 1,
    LOWEST_HEALTH = 2, /* TODO */
};

static vec3_t get_closest_delta(vec3_t viewangles) {
    vec3_t view_height;
    i_engine->pEventAPI->EV_LocalPlayerViewheight(view_height);
    vec3_t local_eyes = vec_add(localplayer->origin, view_height);
    /* TODO: Compensate aim punch */

    /* These 2 vars are used to store the best target across iterations.
     * NOTE: The default value of best_fov is the aimbot fov */
    float best_fov    = 5.0f;
    vec3_t best_delta = { 0, 0, 0 };

    for (int i = 1; i <= i_engine->GetMaxClients(); i++) {
        cl_entity_t* ent = get_player(i);

        if (!valid_player(ent) || !is_alive(ent) || is_friend(ent))
            continue;

        /* TODO: Get bones origin */
        const vec3_t head_pos    = vec_add(ent->origin, vec3(0, 0, 0.6f));
        const vec3_t enemy_angle = vec_to_ang(vec_sub(head_pos, local_eyes));

        const vec3_t delta = vec_sub(enemy_angle, viewangles);
        vec_norm(delta);

        const float fov = hypotf(delta.x, delta.y);

        if (fov < best_fov) {
            best_fov = fov;
            vec_copy(best_delta, delta);
        }
    }

    return best_delta;
}

void aimbot(usercmd_t* cmd) {
    const int setting = cv_aimbot->value;
    if (setting == OFF || !(cmd->buttons & IN_ATTACK))
        return;

    /* Calculate delta with the engine viewangles, not with the cmd ones */
    vec3_t engine_viewangles;
    i_engine->GetViewAngles(engine_viewangles);

    vec3_t best_delta = get_closest_delta(engine_viewangles);
    if (!vec_is_zero(best_delta)) {
        /* NOTE: We can divide the best delta here to add smoothing */

        engine_viewangles.x += best_delta.x;
        engine_viewangles.y += best_delta.y;
        engine_viewangles.z += best_delta.z;
    }

    vec_copy(cmd->viewangles, engine_viewangles);
}
