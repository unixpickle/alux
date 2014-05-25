#include <cstdint>
#include <arch/general/failure.hpp>

extern "C" {

void MbootEntry(void * mbootPtr) {
  
}

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
