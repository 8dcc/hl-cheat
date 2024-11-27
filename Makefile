
# FIXME: Don't use C++ shit, build basic SDK
# FIXME: Remove '-Wno-write-strings', add '-Wpedantic'
CC=g++
INCLUDES=-Isrc/include/sdk/common -Isrc/include/sdk/public -Isrc/include/sdk/pm_shared -Isrc/include/sdk/engine
CFLAGS=-Wall -Wextra -Wno-write-strings -m32 -fPIC $(INCLUDES)
LDFLAGS=-shared
LDLIBS=-lm

SRC=main.c util.c mathutil.c entityutil.c globals.c hooks.c cvars.c detour.c \
	features/movement.c features/esp.c features/chams.c features/aim.c features/misc.c
OBJ=$(addprefix obj/, $(addsuffix .o, $(SRC)))

BIN=libhlcheat.so

# ------------------------------------------------------------------------------

.PHONY: all clean inject

all: $(BIN)

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

inject: $(BIN)
	bash ./inject.sh

# ------------------------------------------------------------------------------

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
