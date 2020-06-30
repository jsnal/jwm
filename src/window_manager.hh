#ifndef WINDOW_MANAGER_HH
#define WINDOW_MANAGER_HH

extern "C" {
  #include <X11/Xlib.h>
}
#include <memory>
#include <unordered_map>

class WindowManager {
  public:
    static ::std::unique_ptr<WindowManager> Create();
    ~WindowManager();
    void Run();

  private:
    WindowManager(Display* display);
    Display* display_;
    const Window root_;

    static int OnXError(Display* display, XErrorEvent* e);
    static int OnWMDetected(Display* display, XErrorEvent* e);
    static bool wm_detected_;

    void OnConfigureRequest(const XConfigureRequestEvent& e);
    void OnMapRequest(const XMapRequestEvent& e);
    void OnUnmapNotify(const XUnmapEvent& e);
    void OnCreateNotify(const XCreateWindowEvent& e);
    void OnConfigureNotify(const XConfigureEvent& e);
    void OnDestroyNotify(const XDestroyWindowEvent& e);
    void OnReparentNotify(const XReparentEvent& e);

    /* Map Helpers */
    void Frame(Window w, bool isPrexisting);
    void Unframe(Window w);

    ::std::unordered_map<Window, Window> clients_;
};

#endif
