
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include <dlfcn.h>    /* dlsym */
#include <link.h>     /* link_map */
#include <unistd.h>   /* getpagesize */
#include <sys/mman.h> /* mprotect */

#include "include/util.h"
#include "include/sdk.h"
#include "include/globals.h"

/*----------------------------------------------------------------------------*/

void engine_draw_text(int x, int y, char* s, rgb_t c) {
    /* Convert to 0..1 range */
    float r = c.r / 255.0f;
    float g = c.g / 255.0f;
    float b = c.b / 255.0f;

    i_engine->pfnDrawSetTextColor(r, g, b);
    i_engine->pfnDrawConsoleString(x, y, s);
}

void draw_tracer(vec3_t start, vec3_t end, rgb_t c, float a, float w,
                 float time) {
    static const char* MDL_STR = "sprites/laserbeam.spr";
    static int beam_idx = i_engine->pEventAPI->EV_FindModelIndex(MDL_STR);

    float r = c.r / 255.f;
    float g = c.g / 255.f;
    float b = c.b / 255.f;

    i_engine->pEfxAPI
      ->R_BeamPoints(start, end, beam_idx, time, w, 0, a, 0, 0, 0, r, g, b);
}

void gl_drawbox(int x, int y, int w, int h, rgb_t c) {
    /* Line width */
    const int lw = 1;

    /*
     *     1
     *   +----+
     * 2 |    | 3
     *   |    |
     *   +----+
     *     4
     */
    gl_drawline(x, y, x + w, y, lw, c);
    gl_drawline(x, y, x, y + h, lw, c);
    gl_drawline(x + w, y, x + w, y + h, lw, c);
    gl_drawline(x, y + h, x + w, y + h, lw, c);
}

void gl_drawline(int x0, int y0, int x1, int y1, float w, rgb_t col) {
    const int alpha = 255;

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(col.r, col.g, col.b, alpha); /* Set colors + alpha */
    glLineWidth(w);                         /* Set line width */
    glBegin(GL_LINES);                      /* Interpret vertices as lines */
    glVertex2i(x0, y0);                     /* Start */
    glVertex2i(x1, y1);                     /* End */
    glEnd();                                /* Stop glBegin, end line mode */
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

/*----------------------------------------------------------------------------*/

void* find_sig(const char* module, const byte* pattern) {
    struct our_link_map {
        /* Base from link.h */
        ElfW(Addr) l_addr;
        const char* l_name;
        ElfW(Dyn) * l_ld;
        struct our_link_map* l_next;
        struct our_link_map* l_prev;

        /* Added */
        struct our_link_map* real;
        long int ns;
        struct libname_list* moduleName;
        ElfW(Dyn) * info[DT_NUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM +
                         DT_ADDRNUM];
        const ElfW(Phdr) * phdr;
    };

    /* TODO: Remove unnecesary cast after porting C++ SDK to C. */
    struct our_link_map* link =
      (struct our_link_map*)dlopen(module, RTLD_NOLOAD | RTLD_NOW);

    if (!link) {
        ERR("Can't open module \"%s\"", module);
        return NULL;
    }

    byte* start = (byte*)link->l_addr;
    byte* end   = start + link->phdr[0].p_memsz;

    dlclose(link);

    const byte* memPos = start;
    const byte* patPos = pattern;

    /*
     * Iterate memory area until *patPos is '\0' (we found pattern).
     * If we start a pattern match, keep checking all pattern positions until we
     * are done or until mismatch. If we find mismatch, reset pattern position
     * and continue checking at the memory location where we started +1
     */
    while (memPos < end && *patPos != '\0') {
        if (*memPos == *patPos || *patPos == '?') {
            memPos++;
            patPos++;
        } else {
            start++;
            memPos = start;
            patPos = pattern;
        }
    }

    /* We reached end of pattern, we found it */
    if (*patPos == '\0')
        return start;

    return NULL;
}

#define PAGE_MASK          (~(PAGE_SZ - 1))
#define PAGE_ALIGN(x)      ((x + PAGE_SZ - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (PAGE_ALIGN(x) - PAGE_SZ)

bool protect_addr(void* ptr, int new_flags) {
    int PAGE_SZ = getpagesize();
    void* p     = (void*)PAGE_ALIGN_DOWN((uint32_t)ptr);

    if (mprotect(p, PAGE_SZ, new_flags) == -1) {
        ERR("Error protecting %p", ptr);
        return false;
    }

    return true;
}
