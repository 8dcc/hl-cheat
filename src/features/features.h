
#ifndef FEATURES_H_
#define FEATURES_H_

#include "../include/sdk.h"

enum visible_flags {
    NONE               = 0,
    ENEMY_VISIBLE      = 1,
    ENEMY_NOT_VISIBLE  = 2,
    FRIEND_VISIBLE     = 3,
    FRIEND_NOT_VISIBLE = 4,
    HANDS              = 5,
};

/*----------------------------------------------------------------------------*/

/* src/features/movement.c */
void bhop(usercmd_t* cmd);

/* src/features/esp.c */
void esp(void);
void correct_movement(usercmd_t* cmd, vec3_t old_angles);

/* src/features/chams.c */
extern visible_flags visible_mode;
bool chams(void* this_ptr);

/* src/features/aim.c */
void aimbot(usercmd_t* cmd);

/* src/features/misc.c */
void custom_crosshair(void);

#endif /* FEATURES_H_ */
