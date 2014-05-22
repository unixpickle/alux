#include "hpet-x64.hpp"

namespace OS {
namespace x64 {

bool SupportsHPET() {
  return ACPI::GetHPETCount() != 0;
}

void InitializeHPET() {
  assert(ACPI::GetHPETCount());
  
  Panic("NYI");
}

}
}