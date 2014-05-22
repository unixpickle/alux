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
  VirtAddr dest;
  bool res = KernMap::Map(desc.address.address, 0x1000, dest);
  assert(res);
  hpetVirtual = (volatile uint64_t *)dest;
  
  uint64_t caps = hpetVirtual[0];
  
  // caps >> 0x20 is the tick period in femptoseconds (10^-15 seconds)
  clockSpeed = 60000000000000000L / (caps >> 0x20);
  
  // enable the main counter
  hpetVirtual[2] |= 1; // ENABLE_CNF
}

uint64_t HPET::GetTime() {
  return hpetVirtual[0x1e];
}

uint64_t HPET::GetTicksPerMin() {
  return clockSpeed;
}

}
}

