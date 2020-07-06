#include "jwm.h"

Display* display;
Window root;

bool JXCreate()
{
  display = XOpenDisplay(NULL);

  if (display == NULL)
  {
    printf("Failed to open X display%s\n", XDisplayName(NULL));
    return false;
  }

  root = DefaultRootWindow(display);
  screen = DefaultScreen(display);

  return true;
}

void JXStart()
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
    printf("Detected another window manager on display%s\n",
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

  for (unsigned int i = 0; i < childrenCount; i++)
    AddClientWindow(childrenReturn[i], true);

  XFree(childrenReturn);
  XUngrabServer(display);

  StartEventListener();
}

int OnXError(Display* display, XErrorEvent* e)
{
  const int MAX_ERROR_TEXT_LENGTH = 1024;
  char error_text[MAX_ERROR_TEXT_LENGTH];
  XGetErrorText(display, e->error_code, error_text, sizeof(error_text));
  printf("Received X error:\n");
  printf("\tRequest: %d - %d\n", (int)e->request_code, e->request_code);
  printf("\tError Code: %d - %s\n", (int)e->error_code, error_text);
  printf("\tResource ID: %ld\n", e->resourceid);
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
