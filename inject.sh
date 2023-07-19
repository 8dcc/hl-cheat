#!/bin/bash

pid=$(pidof "hl_linux")
libpath=$(realpath "libhlcheat.so")

# 0x2 -> RTLD_NOW
sudo gdb -n -q -batch                                   \
    -ex "attach $pid"                                   \
    -ex "set \$dlopen = (void* (*)(char*, int))dlopen"  \
    -ex "set \$dlerror =  (char* (*)(void))dlerror"     \
    -ex "call \$dlopen(\"$libpath\", 2)"                \
    -ex "call \$dlerror()"                              \
    -ex "detach"                                        \
    -ex "quit"

#   -ex "set \$dlclose = (int (*)(void*))dlclose"       \

echo -e "\nDone."
