#include "client.h"

ClientNode* AddClientWindow(Window w, bool isMapped)
{
  XWindowAttributes attr;
  ClientNode *np;

  if (XGetWindowAttributes(display, w, &attr) == 0) return NULL;

  if (attr.override_redirect == True) return NULL;

  np = (ClientNode*) malloc(sizeof(ClientNode));
  memset(np, 0, sizeof(ClientNode));

  np->window = w;
  np->parent = None;
  np->owner  = None;

  np->x = attr.x;
  np->y = attr.y;
  np->width  = attr.width;
  np->height = attr.height;
  np->border_width = attr.border_width;

  np->parent = XCreateSimpleWindow(
      display,
      root,
      attr.x,
      attr.y,
      attr.width,
      attr.height,
      BORDER_WIDTH,
      BORDER_COLOR,
      BACKGROUND_COLOR);

  XSelectInput(
      display,
      np->parent,
      SubstructureRedirectMask | SubstructureNotifyMask);

  XReparentWindow(
      display,
      np->window,
      np->parent,
      0, 0);

  XMapWindow(display, np->parent);

  return np;
}
