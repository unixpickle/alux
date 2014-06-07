#include <arch/x64/multitasking/task-group.hpp>

namespace OS {

namespace x64 {
  
TaskGroup::TaskGroup() {
  addressSpace = NULL; // TODO: create some sort of user address space here
}

TaskGroup::~TaskGroup() {
  delete addressSpace;
}

AddressSpace * TaskGroup::GetAddressSpace() {
  return addressSpace;
}
  
}

}