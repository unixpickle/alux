#ifndef __X64_TASK_HPP__
#define __X64_TASK_HPP__

#include <arch/general/address-space.hpp>

namespace OS {

namespace x64 {

class ArchTask {
public:  
  ArchTask(bool kernel = false);
  ~ArchTask();
  
  AddressSpace & GetAddressSpace();
  
protected:
  AddressSpace * addressSpace;
};

}

typedef x64::ArchTask ArchTask;

}

#endif
