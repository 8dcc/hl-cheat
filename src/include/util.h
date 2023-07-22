#ifndef _UTIL_H
#define _UTIL_H

#include "sdk.h"
#include "globals.h"

/* Vector 2 for 2d points */
typedef float vec2_t[2];

/*----------------------------------------------------------------------------*/

bool is_alive(cl_entity_t* ent);
bool valid_client(cl_entity_t* ent);
char* get_name(int ent_idx);
bool vec_is_zero(vec3_t v);
bool world_to_screen(vec3_t vec, vec2_t screen);

#endif /* _UTIL_H */
