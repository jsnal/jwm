#include "client.h"

#include <X11/Xft/Xft.h>

static void SaveClient(Client* client);
static void CreateDecorations(Client* client);

Client* AddClientWindow(Window w)
{
  XWindowAttributes attr;
  Client* cp;

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

  CreateDecorations(cp);

  XSelectInput(
      display,
      cp->window,
      SubstructureRedirectMask | SubstructureNotifyMask);

  SaveClient(cp);

  XMapWindow(display, cp->window);

  return cp;
}

void CreateDecorations(Client* client)
{
  XSetWindowBorderWidth(display, client->window, BORDER_WIDTH);
  XSetWindowBorder(display, client->window, BORDER_COLOR);
}

void SaveClient(Client* client)
{
  client->next = clients;
  clients = client;
}

void RemoveClientWindow(Client* client)
{
  Client** tc;

  // Remove the right client from the clients list
  for (tc = &clients; *tc && *tc != client; tc = &(*tc)->next);
  *tc = client->next;

  free(client);
}

Client* GetClientFromWindow(Window w)
{
  struct Client* cp;

  for (cp = clients; cp; cp = cp->next)
    if (cp->window == w)
      return cp;

  return NULL;
}
