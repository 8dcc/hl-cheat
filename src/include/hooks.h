
#ifndef _HOOKS_H
#define _HOOKS_H

/*----------------------------------------------------------------------------*/

#include "sdk.h"

/*
 * Table of prefixes:
 *   prefix | meaning
 *   -------+----------------------------
 *   *_t    | typedef (function type)
 *   h_*    | hook function (ours)
 *   ho_*   | hook original (ptr to orig)
 *
 *
 * DECL_HOOK_EXTERN: Version for the header. typedef's the function pointer with
 * the return type, name and args. Example:
 *
 *   DECL_HOOK_EXTERN(int, test, double, double);
 *     typedef int (*test_t)(double, double);
 *     extern test_t ho_test;                  // Original
 *     int h_test(double, double);             // Our func
 *
 *
 * DECL_HOOK: Macro for declaring the global function pointer for the original
 * in the source file. Example:
 *
 *   DECL_HOOK(test);
 *     test_t ho_test = NULL;       // Original
 *
 *
 * HOOK: Macro for storing the original function ptr of an interface and hooking
 * our own. Example:
 *
 *   HOOK(gp_client, CL_CreateMove);
 *     ho_CL_CreateMove = gp_client->CL_CreateMove;     // Original
 *     gp_client->CL_CreateMove = h_CL_CreateMove;      // Our func
 *
 *
 * ORIGINAL: Macro for calling the original function. Example:
 *
 *   ORIGINAL(CL_CreateMove, frametime, cmd, active);
 *     ho_CL_CreateMove(frametime, cmd, active);        // Original
 */
#define DECL_HOOK_EXTERN(type, name, ...)  \
    typedef type (*name##_t)(__VA_ARGS__); \
    extern name##_t ho_##name;             \
    type h_##name(__VA_ARGS__);

#define DECL_HOOK(name) name##_t ho_##name = NULL;

#define HOOK(interface, name)          \
    ho_##name       = interface->name; \
    interface->name = h_##name;

#define ORIGINAL(name, ...) ho_##name(__VA_ARGS__);

/*----------------------------------------------------------------------------*/

bool hooks_init(void);
DECL_HOOK_EXTERN(void, CL_CreateMove, float, usercmd_t*, int);
DECL_HOOK_EXTERN(int, HUD_Redraw, float, int);

#endif /* _HOOKS_H */
