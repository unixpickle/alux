#include "hpet-x64.hpp"

namespace OS {
namespace x64 {

static HPET globalHpet;

bool HPET::IsSupported() {
  return ACPI::GetHPETCount() != 0;
}

void HPET::Initialize() {
  assert(ACPI::GetHPETCount());
  new(&globalHpet) HPET();
}

HPET & HPET::GetGlobal() {
  return globalHpet;
}

HPET::HPET() {
  ACPI::HPETDesc desc = ACPI::GetHPET(0);
  cout << "HPET::HPET() address = " << desc.address.address << endl;
}

uint64_t HPET::GetTime() {
  Panic("HPET::GetTime() NYI");
  return 0;
}

uint64_t HPET::GetTicksPerMin() {
  Panic("HPET::GetTicksPerMin() NYI");
  return 0;
}

}
}

