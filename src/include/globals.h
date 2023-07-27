
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "sdk.h"

/*----------------------------------------------------------------------------*/

/*
 * DECL_INTF: Macro for declaring interfaces and it's originals.
 * DECL_INTF_EXTERN: Extern version for the header.
 *
 * prefix | meaning
 * -------+-------------------------------
 * i_*    | interface ptr (global scope)
 * o_*    | original interface (not a ptr)
 */
#define DECL_INTF(type, name) \
    type* i_##name = NULL;    \
    type o_##name;

#define DECL_INTF_EXTERN(type, name) \
    extern type* i_##name;           \
    extern type o_##name;

/*----------------------------------------------------------------------------*/

extern void* hw;
extern void** h_client; /* hClientDLL hander */
DECL_INTF_EXTERN(cl_enginefunc_t, engine);
DECL_INTF_EXTERN(cl_clientfunc_t, client);
DECL_INTF_EXTERN(playermove_t, pmove);
DECL_INTF_EXTERN(engine_studio_api_t, enginestudio);
DECL_INTF_EXTERN(StudioModelRenderer_t, studiomodelrenderer);

extern game_t* game_info;
extern cl_entity_t* localplayer;

/*----------------------------------------------------------------------------*/

bool globals_init(void);
void globals_store(void);
void globals_restore(void);

#endif /* GLOBALS_H_ */
