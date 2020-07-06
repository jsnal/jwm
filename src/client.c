#include "client.h"

#include <X11/Xft/Xft.h>

static void SaveClient(Client* client);
static void CreateDecorations(Client* client);
static void ManageRaiseFocus(Client* client);
static void ManageArrange(Client* client);

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

  ManageRaiseFocus(cp);
  ManageArrange(cp);

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
  if (!client) return;

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

void ManageArrange(Client* client)
{
  if (!client) return;
  LayoutTile();
}

void LayoutTile()
{
  Client* cp;
  int i, numClients, atY, slaveH, masterW, masterN;

  i = 0;
  numClients = 0;
  atY = 0;

  for (cp = clients; cp; cp = cp->next)
    numClients++;

  Screen *currentScreen = ScreenOfDisplay(display, screen);

  masterW = currentScreen->width / 2;
  masterN = numClients / 2;

  for (cp = clients; cp; cp = cp->next)
  {
    if (numClients == 1)
    {
      cp->x = 0;
      cp->y = 0;
      cp->width = currentScreen->width - (BORDER_WIDTH * 2);
      cp->height = currentScreen->height - (BORDER_WIDTH * 2);
    }
    else
    {
      if (i == masterN)
        atY = 0;

      if (i < masterN)
      {
        cp->x = 0;
        cp->width = masterW;
      }
      else
      {
        cp->x = masterW;
        cp->width = currentScreen->width - masterW - (BORDER_WIDTH * 2);
      }

      cp->y = atY;

      if (i == numClients - 1 || i == masterN - 1)
        slaveH = currentScreen->height - atY;
      else if (i < masterN)
        slaveH = currentScreen->height / masterN;
      else
        slaveH = currentScreen->height / (numClients - masterN);

      cp->height = slaveH - (BORDER_WIDTH * 2);
      atY += slaveH;
    }
    ManageApplySize(cp);
    i++;
  }

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
