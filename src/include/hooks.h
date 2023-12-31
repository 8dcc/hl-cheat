
#ifndef HOOKS_H_
#define HOOKS_H_

/*----------------------------------------------------------------------------*/

#include "sdk.h"
#include "globals.h"

#include <dlfcn.h> /* dlsym */
#include <GL/gl.h> /* GLFloat */
/*
 * Table of prefixes:
 *   prefix | meaning
 *   -------+----------------------------
 *   *_t    | typedef (function type)
 *   h_*    | hook function (ours)
 *   ho_*   | hook original (ptr to orig)
 *   hp_*   | hook pointer (pointer to function pointer)
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
 *
 *
 * GL_HOOK: Hooks a OpenGL function. Example:
 *
 *   GL_HOOK(glColor4f);
 *     void** hp_glColor4f = (void**)dlsym(hw, "qglColor4f"); // Ptr
 *     ho_glColor4f = (glColor4f_t)(*hp_glColor4f);      // Original from ptr
 *     *hp_glColor4f = (void*)h_glColor4f;               // Set ptr to our func

 * Note: ho_glColor4f and h_glColor4f sould be declared with DECL_HOOK_EXTERN
 *
 *
 * GL_UNHOOK: Restores a OpenGL hook created by GL_HOOK. Example:
 *
 *   GL_UNHOOK(glColor4f);
 *     void** hp_glColor4f = (void**)dlsym(hw, "qglColor4f"); // Ptr
 *     *hp_glColor4f = (void*)ho_glColor4f;                   // Set to original
 */
#define DECL_HOOK_EXTERN(TYPE, NAME, ...)  \
    typedef TYPE (*NAME##_t)(__VA_ARGS__); \
    extern NAME##_t ho_##NAME;             \
    TYPE h_##NAME(__VA_ARGS__);

#define DECL_HOOK(NAME) NAME##_t ho_##NAME = NULL;

#define HOOK(INTERFACE, NAME)          \
    ho_##NAME       = INTERFACE->NAME; \
    INTERFACE->NAME = h_##NAME;

#define ORIGINAL(NAME, ...) ho_##NAME(__VA_ARGS__);

#define GL_HOOK(NAME)                                \
    void** hp_##NAME = (void**)dlsym(hw, "q" #NAME); \
    ho_##NAME        = (NAME##_t)(*hp_##NAME);       \
    *hp_##NAME       = (void*)h_##NAME;

#define GL_UNHOOK(NAME)                              \
    void** hp_##NAME = (void**)dlsym(hw, "q" #NAME); \
    *hp_##NAME       = (void*)ho_##NAME;

/*----------------------------------------------------------------------------*/

bool hooks_init(void);
void hooks_restore(void);

/* VMT hooks */
DECL_HOOK_EXTERN(void, CL_CreateMove, float, usercmd_t*, int);
DECL_HOOK_EXTERN(int, HUD_Redraw, float, int);
DECL_HOOK_EXTERN(void, StudioRenderModel, void*);
DECL_HOOK_EXTERN(void, CalcRefdef, ref_params_t*);
DECL_HOOK_EXTERN(void, HUD_PostRunCmd, struct local_state_s*,
                 struct local_state_s*, struct usercmd_s*, int, double,
                 unsigned int);

/* OpenGL hooks */
DECL_HOOK_EXTERN(void, glColor4f, GLfloat, GLfloat, GLfloat, GLfloat);

/* Detour hooks */
DECL_HOOK_EXTERN(void, CL_Move);

#endif /* HOOKS_H_ */
