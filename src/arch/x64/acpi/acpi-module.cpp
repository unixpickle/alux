#include <arch/x64/acpi/acpi-module.hpp>
#include <arch/x64/acpi/sdt.hpp>
#include <arch/x64/acpi/hpet.hpp>
#include <arch/x64/acpi/madt.hpp>
#include <arch/general/global-map.hpp>
#include <memory/malloc.hpp>
#include <new>

namespace OS {

namespace x64 {

static ACPIModule globalModule;
static Module * deps[2];

void ACPIModule::InitGlobal() {
  new(&globalModule) ACPIModule();
}

ACPIModule & ACPIModule::GetGlobal() {
  return globalModule;
}
  
void ACPIModule::Initialize() {
  SDT::Initialize();
  HPETInfo::GetGlobal();
  MADT::Initialize();
}

Module ** ACPIModule::GetDependencies(size_t & count) {
  deps[0] = &Malloc::GetGlobal();
  deps[1] = &GlobalMap::GetGlobal();
  count = 2;
  return deps;
}

};

}

}

#endif
