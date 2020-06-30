extern "C" {
  #include <X11/Xlib.h>
}
#include <memory>

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
    void OnCreateNotify(const XCreateWindowEvent& e);
    void OnDestroyNotify(const XDestroyWindowEvent& e);
    void OnReparentNotify(const XReparentEvent& e);

    /* Map Helpers */
    void Frame(Window w, bool isPrexisting);
};
