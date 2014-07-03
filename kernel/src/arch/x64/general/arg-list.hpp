#ifndef __X64_ARG_LIST_HPP__
#define __X64_ARG_LIST_HPP__

#include <arch/general/arg-list.hpp>

namespace OS {

namespace x64 {

class ArgList : public OS::ArgList {
public:
  ArgList(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
          uint64_t arg5);
  
  virtual bool PopBool();
  virtual int PopInt();
  virtual uint32_t PopUInt32();
  virtual uint64_t PopUInt64();
  virtual PhysAddr PopPhysAddr();
  virtual VirtAddr PopVirtAddr();
  
private:
  uint64_t arguments[5];
  int idx = 0;
};

}

}

#endif
