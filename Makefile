
CC=gcc
CFLAGS=-Wall -Wextra -m32 -fPIC
LDFLAGS=

OBJS=obj/main.c.o
BIN=libhlcheat.so

.PHONY: clean all inject

# -------------------------------------------

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

inject:
	bash ./inject.sh

# -------------------------------------------

# -fPIC (in CFLAGS) and -shared for creating a library (shared object)
# -m32 (in CFLAGS) because of the game's arch
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): obj/%.c.o : src/%.c
	@mkdir -p obj/
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
