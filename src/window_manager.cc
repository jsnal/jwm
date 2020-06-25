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
