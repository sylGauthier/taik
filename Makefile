DEPS := 3dmr

CFLAGS ?= -std=c89 -pedantic -march=native -Wall -g
CFLAGS += $(shell pkg-config --cflags $(DEPS)) -I.
LDFLAGS += $(shell pkg-config --libs $(DEPS)) -lm

OBJECTS := $(patsubst %.c,%.o,$(wildcard src/*.c))

.PHONY: all
all: taik

taik: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(OBJECTS) taik
