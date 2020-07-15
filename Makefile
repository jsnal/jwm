CC = gcc

LIBS = -lX11 -lXrandr
INCLUDES = -I/usr/include/freetype2
CFLAGS = -Wall -O2 -Werror=format-security
CFLAGS += $(LIBS) $(INCLUDES)

all: jwm

release: CFLAGS += -DDEBUG -g
release: jwm

HEADER = \
	 src/jwm.h \
	 src/client.h \
	 src/event.h \
	 src/jwm_constants.h \
	 src/config.h

SOURCES = \
	 src/jwm.c \
	 src/client.c \
	 src/event.c \
	 src/main.c

OBJECTS = $(SOURCES:.c=.o)

jwm: $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f jwm $(OBJECTS)
