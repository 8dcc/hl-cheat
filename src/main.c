#include <stdio.h>

/*
 * We need:
 *   __attribute__((constructor))
 * To indicate that this function will be the entry point once injected.
 */
__attribute__((constructor)) void load(void) {
    printf("hl-cheat loaded!\n");
}
