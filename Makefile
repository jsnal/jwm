CC = gcc

LIBS = -lX11 -lstdc++
CFLAGS = -I. -Wall $(LIBS)

all: jwm

HEADER = \
		src/jwm.h \
		src/client.h \
		src/event.h \
		src/jwm_constants.h

SOURCES = \
		src/jwm.c \
		src/client.c \
		src/event.c \
		src/main.c

EXECUTABLE = $(OUTPUTS:%=$(BIN)/%)
OBJECTS = $(SOURCES:.c=.o)

jwm: $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f jwm $(OBJECTS)
