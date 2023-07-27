
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
