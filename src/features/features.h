
#ifndef FEATURES_H_
#define FEATURES_H_

#include "../include/sdk.h"

/* src/features/movement.c */
void bhop(usercmd_t* cmd);

/* src/features/esp.c */
void esp(void);
void correct_movement(usercmd_t* cmd, vec3_t old_angles);

#endif /* FEATURES_H_ */
