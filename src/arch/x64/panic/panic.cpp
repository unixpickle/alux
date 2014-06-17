#include <arch/x64/panic/panic-module.hpp>
#include <cstdint>

namespace OS {

static bool hasPanicked = false;

void Panic(const char * message) {
  __asm__("cli");
  x64::PanicModule::GetGlobal().DistributeError();
    
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
  __builtin_unreachable();
}

namespace x64 {

void PanicIPI() {
  // don't even bother sending an EOI
  __asm__("cli\nhlt");
}

}

}
