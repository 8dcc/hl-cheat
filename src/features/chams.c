
#include "features.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "../include/util.h"

#include <GL/gl.h>

enum chams_settings {
    DISABLED     = 0,
    ENEMY_CHAMS  = 1,
    FRIEND_CHAMS = 2,
    /* ALL_PLAYER is 3, but we can OR player and hands */
    HAND_CHAMS = 4,
    /* ALL is 5, but we will convert it to 7 so every setting can be OR'd */
};

visible_flags visible_mode;

bool chams(void* this_ptr) {
    const int setting = cv_chams->value == 5.0f ? 7 : cv_chams->value;
    if (setting == DISABLED)
        return false;

    cl_entity_t* ent = i_enginestudio->GetCurrentEntity();

    if (ent->index == localplayer->index && setting & HAND_CHAMS) {
        /* If we are rendering hands and setting is on, render them */
        glDisable(GL_TEXTURE_2D);
        visible_mode = HANDS; /* Set for this call */

        i_studiomodelrenderer->StudioRenderFinal(this_ptr);

        visible_mode = NONE; /* Reset for future calls */
        glEnable(GL_TEXTURE_2D);
        return true;
    } else if (!valid_player(ent) || !is_alive(ent)) {
        /* If we don't want player chams, or this is not a player, stop */
        return false;
    }

    const bool friendly = is_friend(ent);

    if (friendly && !(setting & FRIEND_CHAMS))
        /* Friendly ent but we dont want to render friends */
        return false;
    else if (!friendly && !(setting & ENEMY_CHAMS))
        /* Not friendly ent and we dont want to render enemies */
        return false;

    /* If we got here it means we are rendering a valid player */
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST); /* Ignore depth (walls between target) */
    visible_mode = friendly ? FRIEND_NOT_VISIBLE : ENEMY_NOT_VISIBLE;
    i_studiomodelrenderer->StudioRenderFinal(this_ptr);

    glEnable(GL_DEPTH_TEST); /* Don't ignore depth, different color chams */
    visible_mode = friendly ? FRIEND_VISIBLE : ENEMY_VISIBLE;
    i_studiomodelrenderer->StudioRenderFinal(this_ptr);

    /* Reset for future calls to glColor4f (from here or somewhere else) */
    visible_mode = NONE;
    glEnable(GL_TEXTURE_2D);

    /* StudioRenderModel hook doesn't need to call original */
    return true;
}
