#include "jwm.h"

int main(int argc, char** argv)
{
  if (!Create())
  {
    printf("Failed to init a window manager");
    return 1;
  }

  Start();

  return 0;
}
