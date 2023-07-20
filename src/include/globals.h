#ifndef _GLOBALS_H
#define _GLOBALS_H

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
DECL_INTF_EXTERN(cl_enginefunc_t, engine);
DECL_INTF_EXTERN(cl_clientfunc_t, client);
DECL_INTF_EXTERN(playermove_t, pmove);

/*----------------------------------------------------------------------------*/

bool globals_init(void);

#endif /* _GLOBALS_H */
