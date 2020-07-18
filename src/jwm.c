#include "jwm.h"

#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

static void SetupMonitors();
static void SetupHints();
static Cursor cursor_default;

bool Create()
{
  display = XOpenDisplay(NULL);

  if (display == NULL)
  {
    fprintf(stderr, __WM_NAME__": Failed to open X display%s\n",
            XDisplayName(NULL));
    return false;
  }

  screen = DefaultScreen(display);
  root = RootWindow(display, screen);

  SetupMonitors();
  SetupHints();

  cursor_default = XCreateFontCursor(display, XC_left_ptr);
  XDefineCursor(display, root, cursor_default);

  XChangeProperty(display, root, atomNet[AtomNetWMName],
                  XInternAtom(display, "UTF8_STRING", False), 8,
                  PropModeReplace, (unsigned char *)__WM_NAME__,
                  strlen(__WM_NAME__));

  ManageGrabKeys();

  return true;
}

void Start()
{
  wmFound = false;
  XSetErrorHandler(OnWMDetected);
  XSelectInput(
      display,
      root,
      SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(display, false);

  if (wmFound)
  {
    fprintf(stderr, __WM_NAME__": Detected another window manager on display%s\n",
           XDisplayString(display));
    return;
  }


  XSetErrorHandler(OnXError);
  XGrabServer(display);

  Window rootReturn, parentReturn;
  Window* childrenReturn;
  unsigned int childrenCount;

  XQueryTree(
      display,
      root,
      &rootReturn,
      &parentReturn,
      &childrenReturn,
      &childrenCount
      );

  if (rootReturn != root) return;

  XSelectInput(
      display,
      root,
      SubstructureRedirectMask | SubstructureNotifyMask);

  for (unsigned int i = 0; i < childrenCount; i++)
    AddClientWindow(childrenReturn[i]);

  XFree(childrenReturn);
  XUngrabServer(display);

  StartEventListener();
}

void Destroy()
{
  free(clients);
  free(monitors);
}

void SetupHints()
{
  atomNet[AtomNetSupported] = XInternAtom(display, "_NET_SUPPORTED", False);
  atomNet[AtomNetSupportingWMCheck] = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
  atomNet[AtomNetWMName] = XInternAtom(display, "_NET_WM_NAME", False);
  atomNet[AtomNetWMState] = XInternAtom(display, "_NET_WM_STATE", False);
  atomNet[AtomNetWMStateFullscreen] = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

  atomNet[AtomNetWMWindowType] = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
  atomNet[AtomNetWMWindowTypeDialog] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
  atomNet[AtomNetWMWindowTypeMenu] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_MENU", False);
  atomNet[AtomNetWMWindowTypeSplash] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_SPLASH", False);
  atomNet[AtomNetWMWindowTypeToolbar] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);
  atomNet[AtomNetWMWindowTypeUtility] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", False);

  XChangeProperty(display, root, atomNet[AtomNetSupported], XA_ATOM, 32,
      PropModeReplace, (unsigned char *) atomNet, AtomNetLast);

  atomWM[AtomWMDeleteWindow] = XInternAtom(display, "WM_DELETE_WINDOW", False);
  atomWM[AtomWMProtocols] = XInternAtom(display, "WM_PROTOCOLS", False);
  atomWM[AtomWMState] = XInternAtom(display, "WM_STATE", False);
  atomWM[AtomWMTakeFocus] = XInternAtom(display, "WM_TAKE_FOCUS", False);
}

void SetupMonitors()
{
  XRRMonitorInfo *info;
  int nmon;

  if (!(info = XRRGetMonitors(display, root, True, &nmon))) return;

  /* If for whatever reason a monitor isn't detected */
  if (nmon <= 0 || info == NULL)
  {
    monitors = (Monitor*) calloc(1, sizeof(Monitor));
    monitors[0].wx = monitors[0].mx = 0;
    monitors[0].wy = monitors[0].my = 0;
    monitors[0].ww = monitors[0].mw = 1024;
    monitors[0].wh = monitors[0].mh = 1024;
    return;
  }

  monitors = (Monitor*) calloc(nmon, sizeof(Monitor));
  for (unsigned int i = 0; i < nmon; i++)
  {
    monitors[i].wx = monitors[i].mx = info[i].x;
    monitors[i].wy = monitors[i].my = info[i].y;
    monitors[i].ww = monitors[i].mw = info[i].width;
    monitors[i].wh = monitors[i].mh = info[i].height;
    monitors[i].layout = Tile;
  }
}

int OnXError(Display* display, XErrorEvent* e)
{
  const int MAX_ERROR_TEXT_LENGTH = 1024;
  char error_text[MAX_ERROR_TEXT_LENGTH];
  XGetErrorText(display, e->error_code, error_text, sizeof(error_text));
  fprintf(stderr, __WM_NAME__": Received X error:\n");
  fprintf(stderr, "\tRequest: %d - %d\n", (int)e->request_code, e->request_code);
  fprintf(stderr, "\tError Code: %d - %s\n", (int)e->error_code, error_text);
  fprintf(stderr, "\tResource ID: %ld\n", e->resourceid);
  return 0;
}

int OnXErrorSuppress(Display* display, XErrorEvent* e)
{
  return 0;
}

int OnWMDetected(Display* display, XErrorEvent* e)
{
  if ((int)e->error_code == BadAccess)
  {
    wmFound = true;
  }
  return 1;
}
