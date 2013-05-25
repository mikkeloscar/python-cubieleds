VERSION = $(shell git describe --tags)

CFLAGS := -std=c99 \
		-Wall -Wextra -pedantic \
		${CFLAGS}

all: led

led: led.o leds.o

clean:
	${RM} led *.o

.PHONY: clean install
