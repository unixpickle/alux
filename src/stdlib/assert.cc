#include <cassert>
#include <iostream>
#include <platform/failure.hpp>

extern "C" {

int __assert(const char * msg) {
  OS::Panic(msg);
  return 0;
}

}
