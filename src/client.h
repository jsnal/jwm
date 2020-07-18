#ifndef JWM_CLIENT_H
#define JWM_CLIENT_H

#include "constants.h"
#include "jwm.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <X11/Xatom.h>

/*
 * List of the layouts supported. Based no there place, a call is made to the
 * proper funciton in ManageArrange. The default Layout it Tile.
 */
typedef enum {
  Tile, Stack
} Layouts;

typedef enum {
  AtomNetSupported,
  AtomNetSupportingWMCheck,
  AtomNetWMName,
  AtomNetWMState,
  AtomNetWMStateFullscreen,
  AtomNetWMWindowType,
  AtomNetWMWindowTypeDialog,
  AtomNetWMWindowTypeMenu,
  AtomNetWMWindowTypeSplash,
  AtomNetWMWindowTypeToolbar,
  AtomNetWMWindowTypeUtility,
  AtomNetLast
} AtomsNet;

typedef enum {
  AtomWMDeleteWindow,
  AtomWMProtocols,
  AtomWMState,
  AtomWMTakeFocus,
  AtomWMLast
} AtomsWM;

/*
 * Structure of a Client (window) within the window manager. This is a linked
 * list which means it can be easily traversed using *next. The data within the
 * Client struct must always be the most updated information to work properly.
 */
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
  Layouts ly;
} Arg;

typedef struct Key {
  unsigned int mod;
  KeySym keysym;
  void (*func)(const Arg *);
  const Arg arg;
} Key;

Client* AddClientWindow(Window w);
Client* GetClientFromWindow(Window w);
void RemoveClientWindow(Client* client);
void RemoveDecorations(Client* client);

void ManageFocus(Client* client);
void ManageInputFocus(Client* client);
void ManageApplySize(Client* client);
void ManageArrange(Client* client);
void ManageGrabKeys(t);
int ManageSendEvent(Client* client, Atom atom);

void IOFocusClientWindow(const Arg* arg);
void IOToggleFullscreen(const Arg* arg);
void IOSetLayout(const Arg* arg);
void IOKillClient(const Arg* arg);
void IOSpawn(const Arg* arg);

// Master list of all the known clients
Client* clients;
Monitor* monitors;

/* TODO: Make these values dynamic in some way. Config? */
unsigned int minWindowWidth;
unsigned int minWindowHeight;
unsigned int selmon;

Atom atomNet[AtomNetLast], atomWM[AtomWMLast];
#endif
