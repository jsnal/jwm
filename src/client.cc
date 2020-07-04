#include "client.hh"

ClientNode* AddClientWindow(Window w, bool isMapped)
{
  XWindowAttributes attr;
  ClientNode *np;

  if(XGetWindowAttributes(display, w, &attr) == 0) return nullptr;
}
