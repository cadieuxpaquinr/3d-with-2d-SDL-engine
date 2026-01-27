CC ?= cc
CFLAGS ?= -Wall -Wextra -pedantic -O2
CFLAGS += $(shell sdl2-config --cflags)
LDLIBS += $(shell sdl2-config --libs)
LDLIBS += -lm

TARGET := cube
SRC := cube.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)