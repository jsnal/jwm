#include "event.h"

#include <X11/Xft/Xft.h>

static void HandleConfigureRequest(const XConfigureRequestEvent* event);
static void HandleMapNotify(const XMapRequestEvent* event);
static void HandleUnmapNotify(const XUnmapEvent* event);
static void HandleKeyPress(const XKeyEvent* event);

void StartEventListener()
{
  // TODO: Change this to a do while loop potentially.
  for(;;)
  {
    XEvent event;
    XNextEvent(display, &event);

    switch(event.type) {
      case ConfigureRequest:
        HandleConfigureRequest(&event.xconfigurerequest);
        break;
      case MapRequest:
        HandleMapNotify(&event.xmaprequest);
        break;
      case UnmapNotify:
        HandleUnmapNotify(&event.xunmap);
        break;
      case KeyPress:
        HandleKeyPress(&event.xkey);
        break;
    }
  }
}

void HandleConfigureRequest(const XConfigureRequestEvent* event)
{
  XWindowChanges wc;
  wc.stack_mode = event->detail;
  wc.sibling = event->above;
  wc.border_width = event->border_width;
  wc.x = event->x;
  wc.y = event->y;
  wc.width = event->width;
  wc.height = event->height;

  XConfigureWindow(display, event->window, event->value_mask, &wc);
}

void HandleMapNotify(const XMapRequestEvent* event)
{
  AddClientWindow(event->window);
}

void HandleUnmapNotify(const XUnmapEvent* event)
{
  Client* cp;

  if ((cp = GetClientFromWindow(event->window)))
  {
    RemoveClientWindow(cp);
  }
}

void HandleKeyPress(const XKeyEvent* event)
{
  Client* cp;

  if (!(cp = GetClientFromWindow(event->window))) return;

  if ((event->state & Mod1Mask) &&
      (event->keycode == XKeysymToKeycode(display, XK_Tab))) {
    cp->w = 400;
    cp->h = 400;

    ManageApplySize(cp);
  }
}
