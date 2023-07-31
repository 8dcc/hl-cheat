
#include "features.h"
#include "../include/sdk.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "../include/util.h"

void custom_crosshair(void) {
    if (!CVAR_ON(crosshair))
        return;

    /* Get window size, and then the center. */
    int mx = game_info->m_width / 2;
    int my = game_info->m_height / 2;

    /* The real length is sqrt(2 * (len^2)) */
    const int len   = cv_crosshair->value;
    const int gap   = 1;
    const float w   = 1;
    const rgb_t col = { 255, 255, 255 };

    /*
     *   1\ /2
     *     X
     *   3/ \4
     */
    gl_drawline(mx - gap, my - gap, mx - gap - len, my - gap - len, w, col);
    gl_drawline(mx + gap, my - gap, mx + gap + len, my - gap - len, w, col);
    gl_drawline(mx - gap, my + gap, mx - gap - len, my + gap + len, w, col);
    gl_drawline(mx + gap, my + gap, mx + gap + len, my + gap + len, w, col);
}

void bullet_tracers(usercmd_t* cmd) {
    /* Only draw if we are holding attack and we can shoot */
    if (!CVAR_ON(tracers) || !(cmd->buttons & IN_ATTACK) || !can_shoot() ||
        !is_alive(localplayer))
        return;

    /* Get player eye pos, start of tracer */
    vec3_t view_height;
    i_engine->pEventAPI->EV_LocalPlayerViewheight(view_height);
    vec3_t local_eyes = vec_add(localplayer->origin, view_height);

    /* Get forward vector from viewangles */
    vec3_t fwd;
    i_engine->pfnAngleVectors(cmd->viewangles, fwd, NULL, NULL);

    const int tracer_len = 3000;
    vec3_t end;
    end.x = local_eyes.x + fwd.x * tracer_len;
    end.y = local_eyes.y + fwd.y * tracer_len;
    end.z = local_eyes.z + fwd.z * tracer_len;

    /* NOTE: Change tracer settings here */
    const float w    = 0.8;
    const float time = 2;
    draw_tracer(local_eyes, end, (rgb_t){ 66, 165, 245 }, 1, w, time);
}
