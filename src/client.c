#include "client.h"

#include <X11/Xft/Xft.h>

static void SaveClient(Client* client);
static void CreateDecorations(Client* client);
static void ManageRaiseFocus(Client* client);

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

  XSelectInput(
      display,
      cp->window,
      SubstructureRedirectMask | SubstructureNotifyMask);

  CreateDecorations(cp);

  SaveClient(cp);
  XMapWindow(display, cp->window);

  ManageRaiseFocus(cp);

  XGrabKey(
      display,
      XKeysymToKeycode(display, XK_Tab),
      Mod1Mask,
      cp->window,
      false,
      GrabModeAsync,
      GrabModeAsync);

  return cp;
}

void CreateDecorations(Client* client)
{
  if (client) return;

  XSetWindowBorderWidth(display, client->window, BORDER_WIDTH);
  XSetWindowBorder(display, client->window, BORDER_COLOR);
}

void ManageRaiseFocus(Client* client)
{
  if (!client) return;

  XRaiseWindow(display, client->window);
  XSetInputFocus(display, client->window, RevertToParent, CurrentTime);
}

void ManageApplySize(Client* client)
{
  if (!client) return;

  client->width = client->width < minWindowWidth ? minWindowWidth : client->width;
  client->height = client->height < minWindowHeight ? minWindowHeight : client->height;

  XMoveResizeWindow(display, client->window, client->x, client->y, client->width, client->height);
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
