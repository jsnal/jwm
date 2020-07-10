#ifndef JWM_CLIENT_H
#define JWM_CLIENT_H

#include "constants.h"
#include "jwm.h"

#include <string.h>
#include <stdlib.h>

typedef enum {
  Tile, Stack
} Layouts;

typedef struct Client {
  Window window; // Client window

  int x, y, w, h;
  int old_x, old_y, old_w, old_h;
  int monitor;
  int border_width;

  bool fullscreen;
  bool floating;

  struct Client *next;
} Client;

typedef struct Monitor {
  int currentWorkspace;
  Layouts layout;

  int mx, my, mw, mh; // Actual monitor size
  int wx, wy, ww, wh; // Screen size

  struct Client* focused;
} Monitor;

typedef union Arg {
  int i;
  unsigned int ui;
  float f;
  const void *v;
} Arg;

typedef struct Key {
  unsigned int mod;
  KeySym keysym;
  void (*func)(const Arg *);
  const Arg arg;
} Key;

Client* AddClientWindow(Window w);
void RemoveClientWindow(Client* client);
Client* GetClientFromWindow(Window w);
void RemoveDecorations(Client* client);
void ManageFocus(Client* client);
void ManageInputFocus(Client* client);
void ManageApplySize(Client* client);
void ManageArrange(Client* client);

void IOFocusClientWindow(int d);
void IOToggleFullscreen();

// Master list of all the known clients
Client* clients;
Monitor* monitors;
unsigned int minWindowWidth;
unsigned int minWindowHeight;
unsigned int selmon;
#endif
