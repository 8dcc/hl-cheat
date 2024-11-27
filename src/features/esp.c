
#include <stdint.h>

#include "../include/util.h"
#include "../include/mathutil.h"
#include "../include/entityutil.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "features.h"

/* For cv_esp */
enum esp_values {
    ESP_OFF  = 0,
    ESP_BOX  = 1,
    ESP_NAME = 2,
    /* ESP_ALL should be 3 but we can just OR box and name */
};

bool gl_draw3dbox(vec3_t o, int bh, int bw, int lw) {
    /*
     * Parameters:
     *   bw: 3d box width (game units)
     *   bh: 3d box height (game units)
     *   lw: Line width
     */

    if (vec_is_zero(o))
        return false;

    const rgb_t col = { 255, 255, 255 }; /* Color */

    /*
     * (Origin would be in the middle of the cube)
     *
     *     1---2      y(+)
     *    /|  /|     /
     *   3---4 |    *--x(+)
     *   | 5-|-6    |
     *   |/ O|/     z(-)
     *   7---8
     */
    vec3_t p1 = vec3(o.x - bw / 2, o.y + bw / 2, o.z + bh / 2);
    vec3_t p2 = vec3(o.x + bw / 2, o.y + bw / 2, o.z + bh / 2);
    vec3_t p3 = vec3(o.x - bw / 2, o.y - bw / 2, o.z + bh / 2);
    vec3_t p4 = vec3(o.x + bw / 2, o.y - bw / 2, o.z + bh / 2);
    vec3_t p5 = vec3(o.x - bw / 2, o.y + bw / 2, o.z - bh / 2);
    vec3_t p6 = vec3(o.x + bw / 2, o.y + bw / 2, o.z - bh / 2);
    vec3_t p7 = vec3(o.x - bw / 2, o.y - bw / 2, o.z - bh / 2);
    vec3_t p8 = vec3(o.x + bw / 2, o.y - bw / 2, o.z - bh / 2);

    vec2_t s1, s2, s3, s4, s5, s6, s7, s8;
    if (!world_to_screen(p1, s1) || !world_to_screen(p2, s2) ||
        !world_to_screen(p3, s3) || !world_to_screen(p4, s4) ||
        !world_to_screen(p5, s5) || !world_to_screen(p6, s6) ||
        !world_to_screen(p7, s7) || !world_to_screen(p8, s8))
        return false;

    gl_drawline_points(s1, s2, lw, col);
    gl_drawline_points(s1, s3, lw, col);
    gl_drawline_points(s1, s5, lw, col);
    gl_drawline_points(s2, s4, lw, col);
    gl_drawline_points(s2, s6, lw, col);
    gl_drawline_points(s3, s4, lw, col);
    gl_drawline_points(s3, s7, lw, col);
    gl_drawline_points(s4, s8, lw, col);
    gl_drawline_points(s5, s6, lw, col);
    gl_drawline_points(s5, s7, lw, col);
    gl_drawline_points(s6, s8, lw, col);
    gl_drawline_points(s7, s8, lw, col);

    return true;
}

static bool gl_draw2dbox(vec3_t o, int bh) {
    /* bh: 3d box height (game units) */

    if (vec_is_zero(o))
        return false;

    const rgb_t col     = { 255, 255, 255 }; /* Color */
    const rgb_t out_col = { 0, 0, 0 };       /* Outline */

    /* Get top and bottom of player from origin with box height */
    const vec3_t bot = vec3(o.x, o.y, o.z - bh / 2);
    const vec3_t top = vec3(o.x, o.y, o.z + bh / 2);

    vec2_t s_bot, s_top;
    if (!world_to_screen(bot, s_bot) || !world_to_screen(top, s_top))
        return false;

    const int h = s_bot[1] - s_top[1];
    const int w = bh == 70 ? h * 0.40f : h * 0.75f;
    const int y = s_top[1];
    const int x = s_top[0] - w / 2;

    gl_drawbox(x - 1, y - 1, w + 2, h + 2, out_col); /* Outer outline */
    gl_drawbox(x + 1, y + 1, w - 2, h - 2, out_col); /* Inner outline */
    gl_drawbox(x, y, w, h, col);                     /* Fill */

    return true;
}

void esp(void) {
    const int setting = (int)cv_esp->value;
    if (setting == ESP_OFF)
        return;

    /* Iterate all clients */
    for (int i = 1; i <= i_engine->GetMaxClients(); i++) {
        cl_entity_t* ent = get_player(i);

        if (!valid_player(ent) || !is_alive(ent) || vec_is_zero(ent->origin))
            continue;

        const int bh = (ent->curstate.usehull == 1) ? 44 : 70;
        /* const int bw = 25; */

        /* If ESP_BOX is enabled, draw it. If it returns false, continue */
        if (setting & ESP_BOX && !gl_draw2dbox(ent->origin, bh))
            continue;

        /* Rest of the loop is for name esp, if var is not enabled, continue */
        if (!(setting & ESP_NAME))
            continue;

        /* Draw name on top of the player. */
        vec3_t top = vec3(ent->origin.x, ent->origin.y, ent->origin.z + bh);
        vec2_t s_top;

        if (!world_to_screen(top, s_top))
            continue;

        /* TODO: Instead of -5px, center the player name to the player origin.
         *       I don't know how to get the text size before rendering. */
        engine_draw_text(s_top[0] - 5, s_top[1] - 2, get_name(ent->index),
                         (rgb_t){ 255, 255, 255 });
    }
}
