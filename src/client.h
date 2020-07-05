#ifndef JWM_CLIENT_HH
#define JWM_CLIENT_HH

#include "constants.h"
#include "jwm.h"

#include <string.h>
#include <stdlib.h>

typedef struct ClientNode {
  Window window; // Client window
  Window parent; // Frame window

  Window owner;

  int x, y;
  int width;
  int height;
  int border_width;
} ClientNode;

ClientNode* AddClientWindow(Window w, bool isMapped);

ClientNode* nodes;

#endif
