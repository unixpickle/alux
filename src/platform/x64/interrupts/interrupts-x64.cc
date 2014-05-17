#include <platform/interrupts.hpp>

namespace OS {

bool GetInterruptsEnabled() {
  unsigned long value;
  __asm__("pushfq\n"
          "pop %0" : "=r" (value));
  return (value & (1 << 9)) != 0;
}

void SetInterruptsEnabled(bool flag) {
  if (flag) {
    __asm__("sti");
  } else {
    __asm__("cli");
  }
}

}
