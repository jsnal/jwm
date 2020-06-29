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

  for (;;)
  {
    XEvent e;
    XNextEvent(display_, &e);

    switch (e.type)
    {
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

void WindowManager::OnCreateNotify(const XCreateWindowEvent& e) {}
void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) {}
void WindowManager::OnReparentNotify(const XReparentEvent& e) {}
