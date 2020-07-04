extern "C" {
  #include <X11/Xlib.h>
}
#include <memory>
#include <iostream>

extern Display* display;
extern Window root;

bool JXCreate();
void JXStart();
static int OnXError(Display* display, XErrorEvent* e);
static int OnWMDetected(Display* display, XErrorEvent* e);

static bool wmFound;
