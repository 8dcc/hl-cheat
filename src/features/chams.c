
#include "features.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "../include/util.h"

#include <GL/gl.h>

visible_flags visible_mode;

bool chams(void* this_ptr) {
    if (!CVAR_ON(chams))
        return false;

    cl_entity_t* ent = i_enginestudio->GetCurrentEntity();

    if (ent->index == localplayer->index) {
        /* TODO: Hand chams (set var, check in gl hook, return true) */
        return false;
    } else if (!valid_player(ent) || !is_alive(ent)) {
        return false;
    }

    /* If we got here it means we are rendering a valid player */
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST); /* Ignore depth (walls between target) */
    visible_mode = NOT_VISIBLE;
    i_studiomodelrenderer->StudioRenderFinal(this_ptr);

    glEnable(GL_DEPTH_TEST); /* Don't ignore depth, different color chams */
    visible_mode = VISIBLE;
    i_studiomodelrenderer->StudioRenderFinal(this_ptr);

    /* Reset for future calls to glColor4f (from here or somewhere else) */
    visible_mode = NONE;
    glEnable(GL_TEXTURE_2D);

    /* StudioRenderModel hook doesn't need to call original */
    return true;
}
