#ifndef __X64_TASK_GROUP_HPP__
#define __X64_TASK_GROUP_HPP__

#include <multitasking/task-group.hpp>
#include <arch/general/address-space.hpp>

namespace OS {

namespace x64 {

class TaskGroup : public OS::TaskGroup {
public:
  TaskGroup();
  virtual ~TaskGroup();
  
  AddressSpace * GetAddressSpace();

protected:
  AddressSpace * addressSpace;
};

}

}

#endif