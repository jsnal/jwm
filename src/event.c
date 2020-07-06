#include "event.h"

static void HandleConfigureRequest(const XConfigureRequestEvent* event);
static void HandleMapNotify(const XMapRequestEvent* event);
static void HandleUnmapNotify(const XUnmapEvent* event);

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
