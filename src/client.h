#ifndef JWM_CLIENT_HH
#define JWM_CLIENT_HH

#include "constants.h"
#include "jwm.h"

#include <string.h>
#include <stdlib.h>

typedef struct Client {
  Window window; // Client window

  int x, y;
  int width, height;
  int m_top, m_bottom, m_left, m_right;
  int border_width;

  struct Client *next;
} Client;

Client* AddClientWindow(Window w, bool isMapped);

// Master list of all the known clients
static struct Client *clients = NULL;
#endif
