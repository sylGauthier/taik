DEPS := 3dmr

CFLAGS ?= -std=c89 -pedantic -march=native -Wall -g
CFLAGS += $(shell pkg-config --cflags $(DEPS)) -I.
LDFLAGS += $(shell pkg-config --libs $(DEPS)) -lm

OBJECTS := $(patsubst %.c,%.o,$(wildcard src/*.c))
DATA := data/wall.glb data/ship.glb data/textures/sand.png

.PHONY: all
all: taik $(DATA)

taik: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

data/%:
	mkdir -p data/textures
	wget pedantic.software/syg/files/$* -O $@

clean:
	rm -rf $(OBJECTS) taik
