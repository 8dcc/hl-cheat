#include <stdio.h>
/* #include <dlfcn.h> */

__attribute__((constructor)) void load(void) {
    printf("hl-cheat loaded!\n");
}
