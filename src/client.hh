extern "C" {
  #include <X11/Xlib.h>
}
#include "window_manager.hh"

typedef struct ClientNode {
  Window window; // Client window
  Window parent; // Frame window

  Window owner;

  int x, y;
  int width;
  int height;
  int border_width;
} ClientNode;

class Client {
  public:
    ClientNode* AddClientWindow(Window w, bool isMapped);
};
