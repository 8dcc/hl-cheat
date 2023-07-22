
#include "features.h"
#include "../include/globals.h"
#include "../include/cvars.h"
#include "../include/util.h"

void esp(void) {
    if (!CVAR_ON(esp))
        return;

    /* Iterate all clients */
    for (int i = 1; i <= i_engine->GetMaxClients(); i++) {
        cl_entity_t* ent = i_engine->GetEntityByIndex(i);

        if (!valid_client(ent) || vec_is_zero(ent->origin))
            continue;

        float screen_point[2];
        if (world_to_screen(ent->origin, screen_point)) {
            /* TODO: Draw name at bottom-left of box ESP */
            i_engine->pfnDrawSetTextColor(1, 1, 1);
            i_engine->pfnDrawConsoleString(screen_point[0], screen_point[1],
                                           get_name(ent->index));
        }
    }
}
