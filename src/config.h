#ifndef JWM_CONFIG_HH
#define JWM_CONFIG_HH

#include "jwm.h"

#define MODKEY Mod1Mask

static Key keys[] = {
  { MODKEY, XK_f, IOToggleFullscreen, {0} },
  { MODKEY, XK_j, IOFocusClientWindow, {.i = +1} },
  { MODKEY, XK_k, IOFocusClientWindow, {.i = -1} }
};

#endif
