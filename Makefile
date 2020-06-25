CXXFLAGS ?= -Wall -g
CXXFLAGS += -std=c++1y
CXXFLAGS += `pkg-config --cflags x11 libglog`
LDFLAGS += `pkg-config --libs x11 libglog`

all: jwm

HEADER = \
		src/window_manager.hh

SOURCES = \
		src/main.cc \
		src/window_manager.cc

EXECUTABLE = $(OUTPUTS:%=$(BIN)/%)
OBJECTS = $(SOURCES:.cc=.o)

jwm: $(HEADERS) $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f jwm $(OBJECTS)
