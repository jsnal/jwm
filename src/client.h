#ifndef JWM_CLIENT_H
#define JWM_CLIENT_H

#include "constants.h"
#include "jwm.h"

#include <string.h>
#include <stdlib.h>

typedef struct Client {
  Window window; // Client window

  int x, y;
  int w, h;
  int m_top, m_bottom, m_left, m_right;
  int border_width;

  struct Client *next;
} Client;

typedef struct Monitor {
  int currentWorkspace;

  int mx, my, mw, mh; // Actual monitor size
  int wx, wy, ww, wh; // Screen size
} Monitor;

Client* AddClientWindow(Window w);
void RemoveClientWindow(Client* client);
Client* GetClientFromWindow(Window w);
void ManageApplySize(Client* client);

// Master list of all the known clients
struct Client *clients;
struct Monitor *monitors;
unsigned int minWindowWidth;
unsigned int minWindowHeight;
#endif
