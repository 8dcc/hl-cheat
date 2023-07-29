
#ifndef CVARS_H_
#define CVARS_H_

#include "sdk.h"
#include "globals.h"

#define CVAR_PREFIX  "cv_"
#define CVAR_HACK_ID 0x4000 /* (1<<14) One that is not in use by the game */

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
    cv_##name =                    \
      i_engine->pfnRegisterVariable(CVAR_PREFIX #name, #value, CVAR_HACK_ID);

#define CVAR_ON(name) (cv_##name->value != 0.0f)

/*----------------------------------------------------------------------------*/

DECL_CVAR_EXTERN(bhop);
DECL_CVAR_EXTERN(autostrafe);
DECL_CVAR_EXTERN(aimbot);
DECL_CVAR_EXTERN(esp);
DECL_CVAR_EXTERN(chams);
DECL_CVAR_EXTERN(crosshair);
DECL_CVAR_EXTERN(clmove);

/*----------------------------------------------------------------------------*/

bool cvars_init(void);

#endif /* CVARS_H_ */
