#include "client.h"
#include "config.h"

static void SaveClient(Client* client);
static void CreateDecorations(Client* client);
static bool IsClientOnMonitor(Client* client, Monitor* monitor);
static void LayoutTile(Monitor *monitor);
static void LayoutStack(Monitor *monitor);

unsigned int minWindowWidth = 50;
unsigned int minWindowHeight = 50;

// TODO: Make selecte monitor dynamic based on what monitor the current cient is on.
unsigned int selmon = 0;

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
  cp->w  = attr.width;
  cp->h = attr.height;
  cp->monitor = selmon;
  cp->border_width = attr.border_width;
  cp->fullscreen = false;

  /* CreateDecorations(cp); */

  XSelectInput(
      display,
      cp->window,
      FocusChangeMask);

  SaveClient(cp);
  XMapWindow(display, cp->window);

  ManageArrange(cp);
  ManageFocus(cp);

  /* XGrabKey( */
  /*     display, */
  /*     XKeysymToKeycode(display, XK_Tab), */
  /*     Mod1Mask, */
  /*     cp->window, */
  /*     false, */
  /*     GrabModeAsync, */
  /*     GrabModeAsync); */
  /*  */
  /* XGrabKey( */
  /*     display, */
  /*     XKeysymToKeycode(display, XK_j), */
  /*     Mod1Mask, */
  /*     cp->window, */
  /*     false, */
  /*     GrabModeAsync, */
  /*     GrabModeAsync); */
  /*  */
  /* XGrabKey( */
  /*     display, */
  /*     XKeysymToKeycode(display, XK_k), */
  /*     Mod1Mask, */
  /*     cp->window, */
  /*     false, */
  /*     GrabModeAsync, */
  /*     GrabModeAsync); */
  /*  */
  /* XGrabKey( */
  /*     display, */
  /*     XKeysymToKeycode(display, XK_f), */
  /*     Mod1Mask, */
  /*     cp->window, */
  /*     false, */
  /*     GrabModeAsync, */
  /*     GrabModeAsync); */

  D fprintf(stderr, __WM_NAME__": Client window added: %ld\n",
            cp->window);

  return cp;
}

void CreateDecorations(Client* client)
{
  if (!client) return;

  XSetWindowBorderWidth(display, client->window, FOCUS_BORDER_WIDTH);
  XSetWindowBorder(display, client->window, FOCUS_BORDER_COLOR);
}

void RemoveDecorations(Client* client)
{
  XSetWindowBorderWidth(display, client->window, UNFOCUS_BORDER_WIDTH);
  XSetWindowBorder(display, client->window, UNFOCUS_BORDER_COLOR);
}

void ManageInputFocus(Client* client)
{
  XSetInputFocus(display, client->window, RevertToParent, CurrentTime);
  CreateDecorations(client);
}

void ManageFocus(Client* client)
{
  if (!client) return;

  printf("running manage focus\n");

  // Save the current focused window and the previous focued window;
  monitors[client->monitor].focused = client;

  XRaiseWindow(display, client->window);
  ManageInputFocus(client);
}

void ManageApplySize(Client* client)
{
  if (!client) return;

  client->w = client->w < minWindowWidth ? minWindowWidth : client->w;
  client->h = client->h < minWindowHeight ? minWindowHeight : client->h;

  XMoveResizeWindow(display, client->window, client->x, client->y, client->w, client->h);
}

void ManageFullscreen(Client* client)
{
  Monitor* mp;

  if (!client->fullscreen)
  {
    if (!(mp = &monitors[client->monitor])) return;

    client->fullscreen = true;

    client->old_y = client->y;
    client->old_x = client->x;
    client->old_w = client->w;
    client->old_h = client->h;

    client->y = mp->my;
    client->x = mp->mx;
    client->w = mp->mw;
    client->h = mp->mh;

    RemoveDecorations(client);
    ManageApplySize(client);
  }
}

void ManageUnfullscreen(Client* client)
{
  Monitor* mp;

  if (client->fullscreen)
  {
    if (!(mp = &monitors[client->monitor])) return;

    client->fullscreen = false;

    client->y = client->old_y;
    client->x = client->old_x;
    client->w = client->old_w;
    client->h = client->old_h;

    ManageApplySize(client);
    ManageArrange(client);
    CreateDecorations(client);
  }
}

void ManageArrange(Client* client)
{
  if (!client) return;

  Monitor* mp = &monitors[client->monitor];
  switch(mp->layout)
  {
    case 0:
      LayoutTile(mp);
      break;
    case 1:
      LayoutStack(mp);
      break;
    default:
      LayoutTile(mp);
      break;
  }
}

void ManageGrabKeys(Client* client)
{
  KeyCode code;

  XUngrabKey(display, AnyKey, AnyModifier, root);
  for (unsigned int i = 0; i < LENGTH(keys); i++)
    if ((code = XKeysymToKeycode(display, keys[i].keysym))) {
      XGrabKey(display, code, keys[i].mod, client->window, True,
               GrabModeAsync, GrabModeAsync);
    }
    else
      fprintf(stderr, __WM_NAME__": Error grabbing key %ld\n", keys[i].keysym);
}

void LayoutTile(Monitor* monitor)
{
  Client* cp;
  int i, numClients, atY, slaveH, masterW, masterN;

  i = 0;
  numClients = 0;
  atY = 0;

  for (cp = clients; cp; cp = cp->next)
    numClients++;

  masterW = monitor->ww / 2;
  masterN = numClients / 2;

  for (cp = clients; cp; cp = cp->next)
  {
    if (IsClientOnMonitor(cp, monitor) && !cp->fullscreen)
    {
      if (numClients == 1)
      {
        cp->x = monitor->wx;
        cp->y = monitor->wy;
        cp->w = monitor->ww - (FOCUS_BORDER_WIDTH * 2);
        cp->h = monitor->wh - (FOCUS_BORDER_WIDTH * 2);
      }
      else
      {
        if (i == masterN)
          atY = 0;

        if (i < masterN)
        {
          cp->x = monitor->wx;
          cp->w = masterW;
        }
        else
        {
          cp->x = monitor->wx + masterW;
          cp->w = monitor->ww - masterW - (FOCUS_BORDER_WIDTH * 2);
        }

        cp->y = atY;

        if (i == numClients - 1 || i == masterN - 1)
          slaveH = monitor->wh - atY;
        else if (i < masterN)
          slaveH = monitor->wh  / masterN;
        else
          slaveH = monitor->wh  / (numClients - masterN);

        cp->h = slaveH - (FOCUS_BORDER_WIDTH * 2);
        atY += slaveH;
      }
      ManageApplySize(cp);
      i++;
    }
  }
}

void LayoutStack(Monitor* monitor)
{
  Client* cp;
  int i, numClients, atY, slaveH;

  i = 0;
  numClients = 0;
  atY = 0;

  for (cp = clients; cp; cp = cp->next)
    numClients++;

  for (cp = clients; cp; cp = cp->next)
  {
    if (IsClientOnMonitor(cp, monitor) && !cp->fullscreen)
    {
      cp->x = monitor->wx;
      cp->w = monitor->ww - (FOCUS_BORDER_WIDTH * 2);

      if (i == numClients - 1)
        slaveH = monitor->wh - atY;
      else
        slaveH = monitor->wh / numClients;

      cp->y = monitor->wy + atY;
      cp->h = slaveH - (FOCUS_BORDER_WIDTH * 2);
      atY += slaveH;
      ManageApplySize(cp);
      i++;
    }
  }
}

bool IsClientOnMonitor(Client* client, Monitor* monitor)
{
  return monitor == &monitors[client->monitor];
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

/*
 * Change the focus of the client window based on where you are in the clients
 * linked list. If you input 1, you will step foward one item in the list. If
 * you input -1, you will step backwards one item. This code is heavily inpirsed
 * by dwm's single linked list traverser.
 */
void IOFocusClientWindow(const Arg* arg)
{
  Client* cp = NULL, *icp = NULL;
  Monitor* mp;

  if (!(mp = &monitors[selmon])) return;

  if (arg->i > 0)
  {
    for (cp = mp->focused->next; cp && !IsClientOnMonitor(cp, mp); cp = cp->next);
    if (!cp)
      for (cp = clients; cp && !IsClientOnMonitor(cp, mp); cp = cp->next);
  }
  else
  {
    for (icp = clients; icp != mp->focused; icp = icp->next)
      if (IsClientOnMonitor(icp, mp))
        cp = icp;
    if (!cp)
      for (; icp; icp = icp->next)
        if (IsClientOnMonitor(icp, mp))
          cp = icp;
  }

  if (cp)
  {
    ManageFocus(cp);
  }
}

void IOToggleFullscreen(const Arg* arg)
{
  Client* cp;

  if (!(cp = monitors[selmon].focused)) return;

  if (cp->fullscreen)
    ManageUnfullscreen(cp);
  else
    ManageFullscreen(cp);
}
