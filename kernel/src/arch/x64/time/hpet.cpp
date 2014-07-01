#include <arch/x64/time/hpet.hpp>
#include <arch/x64/acpi/hpet.hpp>
#include <arch/x64/acpi/acpi-module.hpp>
#include <arch/general/global-map.hpp>
#include <memory/malloc.hpp>
#include <new>

namespace OS {

namespace x64 {

static HPET globalHPET;

HPET::HPET() {
  HPETInfo * info = HPETInfo::GetGlobal();
  map = new EasyMap(info->address.address, 0x1000);
  
  // get the clock speed
  volatile uint64_t * regs = (volatile uint64_t *)map->GetStart();
  uint64_t caps = regs[0];
  clockSpeed = 60000000000000000L / (caps >> 0x20);
}

HPET::~HPET() {
  delete map;
}

bool HPET::IsSupported() {
  return HPETInfo::GetGlobal() != NULL;
}

void HPET::Initialize() {
  new(&globalHPET) HPET();
}

HPET & HPET::GetGlobal() {
  return globalHPET;
}

uint64_t HPET::GetTime() {
  volatile uint64_t * regs = (volatile uint64_t *)map->GetStart();
  return regs[0x1e];
}

uint64_t HPET::GetTicksPerMin() {
  return clockSpeed;
}

void HPET::Start() {
  volatile uint64_t * regs = (volatile uint64_t *)map->GetStart();
  regs[2] |= 1;
}

void HPET::Stop() {
  volatile uint64_t * regs = (volatile uint64_t *)map->GetStart();
  regs[2] &= ~(uint64_t)1;
}

}

}
