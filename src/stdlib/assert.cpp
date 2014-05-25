#include <arch/general/failure.hpp>

extern "C" {

int __assert(const char * msg) {
  OS::Panic(msg);
  return 0;
}

}
