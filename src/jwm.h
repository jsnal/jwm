#ifndef JWM_HH
#define JWM_HH

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdbool.h>

extern Display* display;
extern Window root;

bool JXCreate();
void JXStart();
static int OnXError(Display* display, XErrorEvent* e);
static int OnWMDetected(Display* display, XErrorEvent* e);

static bool wmFound;
#endif
