#include <arch-specific/task.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/user-map.hpp>
#include <arch/general/failure.hpp>
#include <scheduler/general/task.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

ArchTask::ArchTask(bool kernel) {
  AssertNoncritical();
  if (!kernel) {
    addressSpace = new UserMap(static_cast<Task *>(this));
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
