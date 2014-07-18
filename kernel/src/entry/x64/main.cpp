#include "main.hpp"

extern "C" {

void AluxMainX64() {
  char * buffer((char *)0xb8000);
  buffer[0] = 0xa;
  buffer[1] = 'h';
  buffer[2] = 'h';
  buffer[3] = 0xa;
  __asm__ __volatile__("cli\nhlt");
}

}
