#include "window_manager.hh"
#include <glog/logging.h>

using ::std::unique_ptr;

bool WindowManager::wm_detected_;

unique_ptr<WindowManager> WindowManager::Create()
{
  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr)
  {
    LOG(ERROR) << "Failed to open X display" << XDisplayName(nullptr);
    return nullptr;
  }

  return unique_ptr<WindowManager>(new WindowManager(display));
}

WindowManager::WindowManager(Display* display)
  : display_(CHECK_NOTNULL(display)),
    root_(DefaultRootWindow(display_))
{
}

WindowManager::~WindowManager()
{
  XCloseDisplay(display_);
}

void WindowManager::Run()
{
  wm_detected_ = false;
  XSetErrorHandler(&WindowManager::OnWMDetected);
  XSelectInput(
      display_,
      root_,
      SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(display_, false);
  if (wm_detected_)
  {
    LOG(ERROR) << "Detected another window manager on display"
               << XDisplayString(display_);
    return;
  }

  XSetErrorHandler(&WindowManager::OnXError);

  XGrabServer(display_);

  Window rRoot, rParent;
  Window* topLevelWindows;
  unsigned int numTopLevelWindows;

  CHECK(XQueryTree(
        display_,
        root_,
        &rRoot,
        &rParent,
        &topLevelWindows,
        &numTopLevelWindows
        ));

  CHECK_EQ(rRoot, root_);

  for (unsigned int i = 0; i < numTopLevelWindows; i++)
  {
    Frame(topLevelWindows[i], true);
  }

  XFree(topLevelWindows);
  XUngrabServer(display_);

  for (;;)
  {
    XEvent e;
    XNextEvent(display_, &e);

    switch (e.type)
    {
      case ConfigureRequest:
        OnConfigureRequest(e.xconfigurerequest);
        break;
      case MapRequest:
        OnMapRequest(e.xmaprequest);
        break;
      case CreateNotify:
        OnCreateNotify(e.xcreatewindow);
        break;
      case DestroyNotify:
        OnDestroyNotify(e.xdestroywindow);
        break;
      case ReparentNotify:
        OnReparentNotify(e.xreparent);
        break;
    }
  }
}

int WindowManager::OnWMDetected(Display* display, XErrorEvent* e)
{
  CHECK_EQ(static_cast<int>(e->error_code), BadAccess);
  wm_detected_ = true;
  return 0;
}

int WindowManager::OnXError(Display* display, XErrorEvent* e)
{
  LOG(ERROR) << e;
  return 0;
}


/*
 * Configure a window with important default parameters. Since the window is
 * invisible the window manager can grant requests directly without direct
 * modifitcation by invoking this function.
 */

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e)
{
  XWindowChanges changes;

  changes.x = e.x;
  changes.y = e.y;
  changes.width = e.width;
  changes.height = e.height;
  changes.border_width = e.border_width;
  changes.sibling = e.above;

  XConfigureWindow(display_, e.window, e.value_mask, &changes);
}

void WindowManager::OnMapRequest(const XMapRequestEvent& e)
{
  Frame(e.window, false);
  XMapWindow(display_, e.window);
}

void WindowManager::OnCreateNotify(const XCreateWindowEvent& e) {}
void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) {}
void WindowManager::OnReparentNotify(const XReparentEvent& e) {}

void WindowManager::Frame(Window w, bool isPrexisting)
{
  const unsigned int BORDER_WIDTH = 3;
  const unsigned long BORDER_COLOR = 0xff0000;
  const unsigned long BG_COLOR = 0x0000ff;

  XWindowAttributes windowAttributes;
  CHECK(XGetWindowAttributes(display_, w, &windowAttributes));

  if (isPrexisting)
    if (windowAttributes.override_redirect
        || windowAttributes.map_state != IsViewable)
      return;

  const Window frame = XCreateSimpleWindow(
      display_,
      root_,
      windowAttributes.x,
      windowAttributes.y,
      windowAttributes.width,
      windowAttributes.height,
      BORDER_WIDTH,
      BORDER_COLOR,
      BG_COLOR
      );

  XSelectInput(
      display_,
      frame,
      SubstructureRedirectMask | SubstructureNotifyMask
      );

  XAddToSaveSet(display_, w);

  XReparentWindow(
      display_,
      w,
      frame,
      0, 0
      );

  XMapWindow(display_, frame);
}
