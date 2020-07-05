#include "client.h"

#include <X11/Xft/Xft.h>

static void SaveClient(Client* client);
static Client* GetClientFromWindow(Window w);
static void CreateDecorations(Client* client);

Client* AddClientWindow(Window w, bool isMapped)
{
  XWindowAttributes attr;
  Client *cp;

  if (XGetWindowAttributes(display, w, &attr) == 0 ||
      attr.override_redirect == True ||
      GetClientFromWindow(w))
    return NULL;

  cp = (Client*) malloc(sizeof(Client));

  cp->window = w;

  cp->x = attr.x;
  cp->y = attr.y;
  cp->width  = attr.width;
  cp->height = attr.height;
  cp->border_width = attr.border_width;

  XSetWindowBorder(display, cp->window, 0);

  XSelectInput(
      display,
      cp->window,
      SubstructureRedirectMask | SubstructureNotifyMask);

  SaveClient(cp);

  XMapWindow(display, cp->window);

  return cp;
}

void CreateDecorations(Client *client)
{
  XSetWindowAttributes wa = {
    .override_redirect = True,
    .event_mask = ExposureMask,
  };
  XClassHint ch = {
    .res_class = "JWM", // TODO: Put in a MACRO
    .res_name = "decoration",
  };

}

void SaveClient(Client *client)
{
  client->next = clients;
  clients = client;
}

Client* GetClientFromWindow(Window w)
{
  struct Client* cp;

  for (cp = clients; cp; cp = cp->next)
    if (cp->window == w)
      return cp;

  return NULL;
}
