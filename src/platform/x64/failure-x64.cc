#include <platform/failure.h>
#include <cstddef>

namespace OS {

void Panic(const char * message) {
  // TODO: here, broadcast the failure message to every CPU
  __asm__("cli\n"
          "hlt");
}

void ForceHalt() {
  // TODO: here, send a halt interrupt to every CPU and then use ACPI
  // shutdown.
  Panic(NULL);
}

}
