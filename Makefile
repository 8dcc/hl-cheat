
# Need to use g++ because the sdk headers use classes
CC=g++
INCLUDES=-Isrc/include/sdk/common -Isrc/include/sdk/public -Isrc/include/sdk/pm_shared -Isrc/include/sdk/engine
CFLAGS=-Wall -Wextra -Wno-write-strings -m32 -fPIC $(INCLUDES)
LDFLAGS=-lm

OBJS=obj/main.c.o obj/globals.c.o obj/cvars.c.o obj/hooks.c.o obj/util.c.o obj/features/movement.c.o obj/features/esp.c.o
BIN=libhlcheat.so

.PHONY: clean all inject

# -------------------------------------------

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

inject: $(BIN)
	bash ./inject.sh

# -------------------------------------------

# -fPIC (in CFLAGS) and -shared for creating a library (shared object)
# -m32 (in CFLAGS) because of the game's arch
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): obj/%.c.o : src/%.c
	@mkdir -p obj/features/
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
