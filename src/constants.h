#ifndef JWM_CONSTANTS_HH
#define JWM_CONSTANTS_HH

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef DEBUG
#define D if (true)
#else
#define D if (false)
#endif

#define __WM_NAME__  "JWM"

/* Framing Constants */
#define FOCUS_BORDER_WIDTH 2
#define FOCUS_BORDER_COLOR 0xcc0000
#define UNFOCUS_BORDER_WIDTH 0
#define UNFOCUS_BORDER_COLOR 0x0000cc
#define BACKGROUND_COLOR 0x0000ff

#endif
