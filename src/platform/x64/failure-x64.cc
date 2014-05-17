#include <platform/failure.hpp>
#include <cstddef>
#include <iostream>

namespace OS {

void Panic(const char * message) {
  cerr << message;
  // TODO: here, broadcast the failure message to every CPU
  __asm__("cli\n"
          "hlt");
}

void ForceHalt() {
  // TODO: here, send a halt interrupt to every CPU and then use ACPI
  // shutdown.
  Panic("FORCED HALT");
}

}
