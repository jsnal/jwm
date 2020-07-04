CC = gcc
CFLAGS = -I. -Wall
CFLAGS += `pkg-config --cflags x11`
LDFLAGS += `pkg-config --libs x11`

all: jwm

HEADER = \
		src/jwm.h \
		src/client.h \
		src/jwm_constants.h

SOURCES = \
		src/main.c \
		src/jwm.c \
		src/client.c

EXECUTABLE = $(OUTPUTS:%=$(BIN)/%)
OBJECTS = $(SOURCES:.c=.o)

jwm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f jwm $(OBJECTS)
