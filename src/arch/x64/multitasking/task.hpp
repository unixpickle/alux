#ifndef __X64_TASK_HPP__
#define __X64_TASK_HPP__

#include <multitasking/task.hpp>
#include <arch/general/address-space.hpp>

namespace OS {

namespace x64 {

class Task : public OS::Task {
public:
  Task();
  virtual ~Task();
  
  AddressSpace * GetAddressSpace();

protected:
  AddressSpace * addressSpace;
};

}

}

#endif