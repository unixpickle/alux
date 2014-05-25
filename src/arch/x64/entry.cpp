#include <cstdint>

extern "C" {

void MbootEntry(void * mbootPtr) {
  // TODO: something here
  volatile uint16_t * buffer = (uint16_t *)0xb8000;
  buffer[0] = 0xa00 + 'h';
  buffer[1] = 0xa00 + 'e';
  buffer[2] = 0xa00 + 'l';
  buffer[3] = 0xa00 + 'l';
  buffer[4] = 0xa00 + 'o';
  __asm__("hlt");
}

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
