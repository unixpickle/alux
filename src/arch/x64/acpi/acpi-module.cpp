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

DepList ACPIModule::GetDependencies() {
  return DepList(&Malloc::GetGlobal(), &GlobalMap::GetGlobal());
}

};

}

}

#endif
