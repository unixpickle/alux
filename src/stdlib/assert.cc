#include <cassert>
#include <iostream>

extern "C" {

void __assert(const char * msg, const char * file, int line) {
  OS::cerr << "Assertion failure: " << msg << " at " << file
    << ":" << line << OS::endl;
  OS::Panic("assertion failure in kernel.");
}

}
