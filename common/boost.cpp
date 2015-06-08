#include <exception>
#include <cassert>
#include <cstdlib>

using namespace std;

namespace boost
{
  // Exceptionless default exception handler for boost purposes.
  // We don't expect to ever get here.

  void
  throw_exception(std::exception const &e)
  {
    assert(0);
    exit(0);
  }
}

