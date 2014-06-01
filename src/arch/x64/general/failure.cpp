#include <cstdint>

namespace OS {

static bool useIPI = false;

void InitializePanic() {
  useIPI = true;
}

void Panic(const char * message) {
  // TODO: here, if useIPI is true, use an IPI to halt the other CPUs
  __asm__("cli");
    
  // write the error to the screen
  uint16_t * buf = (uint16_t *)0xb8000;
  int i = 0;
  const char * prefix = "PANIC: ";
  while (*prefix) {
    buf[i++] = *prefix | 0xf200;
    prefix++;
  }
  while (*message) {
    buf[i++] = *message | 0xf900;
    message++;
  }
  buf[i] = 0xf920;
  
  __asm__("hlt");
}

}
