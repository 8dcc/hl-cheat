
#ifndef ENTITY_H_
#define ENTITY_H_ 1

#include "sdk.h"

cl_entity_t* get_player(int ent_idx);
bool is_alive(cl_entity_t* ent);
bool valid_player(cl_entity_t* ent);
bool is_friend(cl_entity_t* ent);
bool can_shoot(void);
char* get_name(int ent_idx);

#endif /* ENTITY_H_ */
