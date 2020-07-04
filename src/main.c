#include "jwm.h"

int main(int argc, char** argv)
{
  if (!JXCreate())
  {
    printf("Failed to init a window manager");
    return 1;
  }

  JXStart();

  return 0;
}
