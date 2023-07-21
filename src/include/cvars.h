
#ifndef _CVARS_H
#define _CVARS_H

#include "sdk.h"
#include "globals.h"

#define CVAR_PREFIX  "cv_"
#define CVAR_HACK_ID 0x2000 /* (1<<13) */

/*
 *  DECL_CVAR: Declares cvar variable in source file.
 *  DECL_CVAR_EXTERN: Same but for headers.
 *  REGISTER_CVAR: Create the cvar, return cvar_t*
 *  CVAR_ON: Returns true if the cvar is non-zero
 *
 * prefix | meaning
 * -------+-------------------------------
 * cv_*   | cvar variable
 */
#define DECL_CVAR(name) cvar_t* cv_##name = NULL;

#define DECL_CVAR_EXTERN(name) extern cvar_t* cv_##name;

#define REGISTER_CVAR(name, value) \
    i_engine->pfnRegisterVariable(CVAR_PREFIX name, value, CVAR_HACK_ID);

#define CVAR_ON(name) (cv_##name->value != 0)

/*----------------------------------------------------------------------------*/

DECL_CVAR_EXTERN(bhop);

/*----------------------------------------------------------------------------*/

bool cvars_init(void);

#endif /* _CVARS_H */
