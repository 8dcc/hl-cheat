
#ifndef HOOKS_H_
#define HOOKS_H_

/*----------------------------------------------------------------------------*/

#include "sdk.h"

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
#define DECL_HOOK_EXTERN(type, name, ...)  \
    typedef type (*name##_t)(__VA_ARGS__); \
    extern name##_t ho_##name;             \
    type h_##name(__VA_ARGS__);

#define DECL_HOOK(name) name##_t ho_##name = NULL;

#define HOOK(interface, name)          \
    ho_##name       = interface->name; \
    interface->name = h_##name;

#define ORIGINAL(name, ...) ho_##name(__VA_ARGS__);

#define GL_HOOK(name)                                \
    void** hp_##name = (void**)dlsym(hw, "q" #name); \
    ho_##name        = (name##_t)(*hp_##name);       \
    *hp_##name       = (void*)h_##name;

#define GL_UNHOOK(name)                              \
    void** hp_##name = (void**)dlsym(hw, "q" #name); \
    *hp_##name       = (void*)ho_##name;

/*----------------------------------------------------------------------------*/

bool hooks_init(void);
DECL_HOOK_EXTERN(void, CL_CreateMove, float, usercmd_t*, int);
DECL_HOOK_EXTERN(int, HUD_Redraw, float, int);
DECL_HOOK_EXTERN(void, StudioRenderModel, void* this_ptr);

DECL_HOOK_EXTERN(void, glColor4f, GLfloat r, GLfloat g, GLfloat b, GLfloat a);

#endif /* HOOKS_H_ */
