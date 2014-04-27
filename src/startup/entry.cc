#include <platform/failure.h>
#include <platform/printing.h>

extern "C" {

void EntryPoint() {
  OS::InitializePrinting();
  OS::PrintString("EntryPoint() has been called!");
}

void _EntryPoint() {
  // incase the user is not on a Mac, the one starting with _ will be called.
  EntryPoint();
}

}
