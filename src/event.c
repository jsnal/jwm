#include "event.h"
#include "config.h"

#include <X11/Xft/Xft.h>

static void HandleConfigureRequest(const XConfigureRequestEvent* event);
static void HandleMapNotify(const XMapRequestEvent* event);
static void HandleUnmapNotify(const XUnmapEvent* event);
static void HandleKeyPress(const XKeyEvent* event);
static void HandleFocusIn(const XFocusInEvent* event);
static void HandleFocusOut(const XFocusOutEvent* event);

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
      case FocusIn:
        HandleFocusIn(&event.xfocus);
        break;
      case FocusOut:
        HandleFocusOut(&event.xfocus);
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
  Client* cp;
  if (!(cp = AddClientWindow(event->window)))
  {
    D fprintf(stderr, __WM_NAME__": Failed to map window\n");
    return;
  }
  ManageInputFocus(cp);
  ManageGrabKeys(cp);
}

void HandleUnmapNotify(const XUnmapEvent* event)
{
  Client* cp;

  if ((cp = GetClientFromWindow(event->window)))
  {
    RemoveClientWindow(cp);
    ManageInputFocus(cp->next);
  }
}

void HandleKeyPress(const XKeyEvent* event)
{
  Client* cp;

  if (!(cp = GetClientFromWindow(event->window))) return;

  KeySym keysym;
  keysym = XKeysymToKeycode(display, (KeyCode) event->keycode);
  fprintf(stderr, "Keycode %ld\n", keysym);

  for (unsigned int i = 0; i < LENGTH(keys); i++) {

    fprintf(stderr, "Keycode %ld\n", keys[i].keysym);
    if (keysym == keys[i].keysym && keys[i].func && (event->state & keys[i].mod)) {
      fprintf(stderr, "Keycode %ld\n", keys[i].keysym);
      keys[i].func(&(keys[i].arg));
    }
  }

  /* if ((event->state & Mod1Mask) && */
  /*     (event->keycode == XKeysymToKeycode(display, XK_Tab))) */
  /* { */
  /*   ManageArrange(cp); */
  /* } */
  /* else if ((event->state & Mod1Mask) && */
  /*          (event->keycode == XKeysymToKeycode(display, XK_j))) */
  /* { */
  /*   IOFocusClientWindow(1); */
  /* } */
  /* else if ((event->state & Mod1Mask) && */
  /*          (event->keycode == XKeysymToKeycode(display, XK_k))) */
  /* { */
  /*   IOFocusClientWindow(-1); */
  /* } */
  /* else if ((event->state & Mod1Mask) && */
  /*          (event->keycode == XKeysymToKeycode(display, XK_f))) */
  /* { */
  /*   IOToggleFullscreen(); */
  /* } */
}

void HandleFocusIn(const XFocusInEvent* event)
{
  D fprintf(stderr, __WM_NAME__": Window going in focus: %ld\n",
            event->window);
  Client* cp;

  if (!(cp = GetClientFromWindow(event->window))) return;

  if (!cp->fullscreen)
    ManageFocus(cp);
}

void HandleFocusOut(const XFocusOutEvent* event)
{
  Client* cp;

  D fprintf(stderr, __WM_NAME__": Window going out of focus: %ld\n",
            event->window);

  if (!(cp = GetClientFromWindow(event->window))) return;
  RemoveDecorations(cp);
}
