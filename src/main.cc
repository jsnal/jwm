#include <cstdlib>
#include "jwm.hh"

using ::std::unique_ptr;

int main(int argc, char** argv)
{
  if (!JXCreate())
  {
    std::cout << "Failed to init window manager" << std::endl;
    return EXIT_FAILURE;
  }

  JXStart();

  return EXIT_SUCCESS;
}
