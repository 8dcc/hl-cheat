
#include <dlfcn.h> /* dlsym() */
#include <math.h>

#include "include/sdk.h"
#include "include/util.h"
#include "include/entityutil.h"

cl_entity_t* get_player(int ent_idx) {
    if (ent_idx < 0 || ent_idx > 32)
        return NULL;

    cl_entity_t* ent = i_engine->GetEntityByIndex(ent_idx);

    if (!valid_player(ent))
        return NULL;

    return ent;
}

bool is_alive(cl_entity_t* ent) {
    return ent && ent->curstate.movetype != 6 && ent->curstate.movetype != 0;
}

bool valid_player(cl_entity_t* ent) {
    return ent && ent->player && ent->index != localplayer->index &&
           ent->curstate.messagenum >= localplayer->curstate.messagenum;
}

bool is_friend(cl_entity_t* ent) {
    if (!ent)
        return false;

    /* Check the current game because this method only works for some games */
    switch (this_game_id) {
        case TF: {
            extra_player_info_t* info = (extra_player_info_t*)player_extra_info;

            return info[ent->index].teamnumber ==
                   info[localplayer->index].teamnumber;
        }
        case CS: {
            extra_player_info_cs_t* info =
              (extra_player_info_cs_t*)player_extra_info;

            return info[ent->index].teamnumber ==
                   info[localplayer->index].teamnumber;
        }
        case DOD: {
            extra_player_info_dod_t* info =
              (extra_player_info_dod_t*)player_extra_info;

            return info[ent->index].teamnumber ==
                   info[localplayer->index].teamnumber;
        }
        case HL:
        default:
            return false;
    }
}

bool can_shoot(void) {
    return g_iClip > 0 && g_flNextAttack <= 0.0f &&
           g_flNextPrimaryAttack <= 0.0f;
}

char* get_name(int ent_idx) {
    hud_player_info_t info;
    i_engine->pfnGetPlayerInfo(ent_idx, &info);

    return info.name;
}

game_id get_cur_game(void) {
    typedef void (
      *COM_ParseDirectoryFromCmd_t)(const char*, char*, int, const char*);
    COM_ParseDirectoryFromCmd_t COM_ParseDirectoryFromCmd =
      (COM_ParseDirectoryFromCmd_t)dlsym(hw, "COM_ParseDirectoryFromCmd");

    char game[FILENAME_MAX];
    COM_ParseDirectoryFromCmd("-game", game, sizeof(game), "valve");

    /* Get the current game we are playing */
    if (game[0] == 'c' && game[1] == 's') /* cstrike */
        return CS;
    else if (*game == 'd') /* dod */
        return DOD;
    else if (*game == 't') /* tfc */
        return TF;
    else
        return HL;
}
