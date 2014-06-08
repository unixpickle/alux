#include <arch/x64/multitasking/task.hpp>

namespace OS {

namespace x64 {
  
Task::Task() {
  addressSpace = NULL; // TODO: create some sort of user address space here
}

Task::~Task() {
  delete addressSpace;
}

AddressSpace * Task::GetAddressSpace() {
  return addressSpace;
}
  
}

}