#include <arch-specific/task.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/general/failure.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

ArchTask::ArchTask(bool kernel) {
  AssertNoncritical();
  if (!kernel) {
    // TODO: here, create the address space with something I haven't
    // implemented yet
    Panic("ArchTask(false) - user tasks NYI");
    addressSpace = NULL;
  } else {
    addressSpace = &GlobalMap::GetGlobal();
  }
}

ArchTask::~ArchTask() {
  AssertNoncritical();
  if (addressSpace != &GlobalMap::GetGlobal()) {
    if (addressSpace) delete addressSpace;
  }
}

AddressSpace * ArchTask::GetAddressSpace() {
  return addressSpace;
}

}

}
