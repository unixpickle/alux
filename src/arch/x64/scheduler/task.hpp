#ifndef __X64_TASK_HPP__
#define __X64_TASK_HPP__

#include <scheduler/base/task.hpp>
#include <arch/general/address-space.hpp>

namespace OS {

namespace x64 {

class Task : public OS::Task {
public:
  static Task * New(bool kernel = false);
  
  Task(bool kernel = false);
  virtual ~Task();
  virtual void Delete();
  
  virtual AddressSpace * GetAddressSpace();
  
protected:
  AddressSpace * addressSpace;
};

}

}

#endif
