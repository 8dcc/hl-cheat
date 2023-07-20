#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "sdk.h"

/*----------------------------------------------------------------------------*/

/*
 * DECL_INTF: Macro for declaring interfaces and it's originals.
 * DECL_INTF_EXTERN: Extern version for the header.
 *
 * prefix | meaning
 * -------+----------------
 * gp_*   | global pointer
 * go_*   | global original
 */
#define DECL_INTF(type, name) \
    type* gp_##name = NULL;   \
    type go_##name;

#define DECL_INTF_EXTERN(type, name) \
    extern type* gp_##name;          \
    extern type go_##name;

/*----------------------------------------------------------------------------*/

extern void* hw;
DECL_INTF_EXTERN(cl_enginefunc_t, engine);
DECL_INTF_EXTERN(cl_clientfunc_t, client);

/*----------------------------------------------------------------------------*/

bool globals_init(void);

#endif /* _GLOBALS_H */
