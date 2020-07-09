#include "jwm.h"

int main(int argc, char** argv)
{
  D fprintf(stdout, __WM_NAME__": Warning DEBUG is on\n");

  if (!Create())
  {
    fprintf(stderr, __WM_NAME__": Failed to init a window manager\n");
    exit(EXIT_FAILURE);
  }

  Start();
  Destroy();
  return 0;
}
