#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "sdk.h"

/*----------------------------------------------------------------------------*/

/*
 * prefix | meaning
 * -------+----------------
 * gp_*   | global pointer
 * go_*   | global original
 */
#define DECLARE_SYMBOLS(type, name) \
    type* gp_##name = NULL;         \
    type go_##name;

#define DECLARE_EXTERNS(type, name) \
    extern type* gp_##name;         \
    extern type go_##name;

/*----------------------------------------------------------------------------*/

extern void* hw;
DECLARE_EXTERNS(cl_enginefunc_t, engine);
DECLARE_EXTERNS(cl_clientfunc_t, client);

/*----------------------------------------------------------------------------*/

bool globals_init(void);

#endif /* _GLOBALS_H */
