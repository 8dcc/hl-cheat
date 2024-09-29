/**
 * @file      detour.c
 * @brief     Detour hooking library source
 * @author    8dcc
 *
 * https://github.com/8dcc/detour-lib
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>   /* getpagesize */
#include <sys/mman.h> /* mprotect */

#include "include/detour.h"

#define PAGE_SIZE          getpagesize()
#define PAGE_MASK          (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(x)      ((x + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(x) (PAGE_ALIGN(x) - PAGE_SIZE)

static bool protect_addr(void* ptr, int new_flags) {
    void* p  = (void*)PAGE_ALIGN_DOWN((detour_ptr_t)ptr);
    int pgsz = getpagesize();

    if (mprotect(p, pgsz, new_flags) == -1)
        return false;

    return true;
}

/*
 * 64 bits:
 *   0:  48 b8 45 55 46 84 45    movabs rax,0x454584465545
 *   7:  45 00 00
 *   a:  ff e0                   jmp    rax
 *
 * 32 bits:
 *   0:  b8 01 00 00 00          mov    eax,0x1
 *   5:  ff e0                   jmp    eax
 */
#ifdef __i386__
static uint8_t def_jmp_bytes[] = { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };
#define JMP_BYTES_PTR 1 /* Offset inside the array where the ptr should go */
#else
static uint8_t def_jmp_bytes[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };
#define JMP_BYTES_PTR 2 /* Offset inside the array where the ptr should go */
#endif

void detour_init(detour_data_t* data, void* orig, void* hook) {
    data->detoured = false;
    data->orig     = orig;
    data->hook     = hook;

    /* Store the first N bytes of the original function, where N is the size of
     * the jmp instructions */
    memcpy(data->saved_bytes, orig, sizeof(data->saved_bytes));

    /* Default jmp bytes */
    memcpy(data->jmp_bytes, &def_jmp_bytes, sizeof(def_jmp_bytes));

    /* JMP_BYTES_PTR is defined below def_jmp_bytes, and it changes depending
     * on the arch.
     * We use "&hook" and not "hook" because we want the address of
     * the func, not the first bytes of it like before. */
    memcpy(&data->jmp_bytes[JMP_BYTES_PTR], &hook, sizeof(detour_ptr_t));
}

bool detour_add(detour_data_t* d) {
    /* Already detoured, nothing to do */
    if (d->detoured)
        return true;

    /* See util.c */
    if (!protect_addr(d->orig, PROT_READ | PROT_WRITE | PROT_EXEC))
        return false;

    /* Copy our jmp instruction with our hook address to the orig */
    memcpy(d->orig, d->jmp_bytes, sizeof(d->jmp_bytes));

    /* Restore old protection */
    if (protect_addr(d->orig, PROT_READ | PROT_EXEC)) {
        d->detoured = true;
        return true;
    }

    return false;
}

bool detour_del(detour_data_t* d) {
    /* Not detoured, nothing to do */
    if (!d->detoured)
        return true;

    /* See util.c */
    if (!protect_addr(d->orig, PROT_READ | PROT_WRITE | PROT_EXEC))
        return false;

    /* Restore the bytes that were at the start of orig (we saved on init) */
    memcpy(d->orig, d->saved_bytes, sizeof(d->saved_bytes));

    /* Restore old protection */
    if (protect_addr(d->orig, PROT_READ | PROT_EXEC)) {
        d->detoured = false;
        return true;
    }

    return false;
}
