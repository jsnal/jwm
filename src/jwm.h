#ifndef JWM_HH
#define JWM_HH

#include "constants.h"
#include "event.h"

Display* display;
Window root;
int screen;

bool JXCreate();
void JXStart();
int OnXError(Display* display, XErrorEvent* e);
int OnWMDetected(Display* display, XErrorEvent* e);

bool wmFound;
#endif
