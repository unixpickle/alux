#include <arch/x64/scheduler/task.hpp>
#include <arch/x64/vmm/global-map.hpp>

namespace OS {

namespace x64 {

Task * Task::New(bool kernel) {
  // TODO: use some sort of slab here
  return new Task(kernel);
}

Task::Task(bool kernel) {
  if (!kernel) {
    // TODO: here, create the address space with something I haven't
    // implemented yet
    addressSpace = NULL;
  } else {
    addressSpace = &GlobalMap::GetGlobal();
  }
}

Task::~Task() {
  if (addressSpace != &GlobalMap::GetGlobal()) {
    if (addressSpace) delete addressSpace;
  }
}

void Task::Delete() {
  // TODO: use some sort of slab here
  delete this;
}

AddressSpace * Task::GetAddressSpace() {
  return addressSpace;
}

}

}