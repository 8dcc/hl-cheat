
# Need to use g++ because the sdk headers use classes
CC=g++
INCLUDES=-Isrc/sdk/common -Isrc/sdk/public -Isrc/sdk/pm_shared -Isrc/sdk/engine
CFLAGS=-Wall -Wextra -Wno-write-strings -m32 -fPIC $(INCLUDES)
LDFLAGS=

OBJS=obj/main.c.o obj/globals.c.o
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
	@mkdir -p obj/
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
