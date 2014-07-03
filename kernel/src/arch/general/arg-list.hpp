#ifndef __GENERAL_ARG_LIST_HPP__
#define __GENERAL_ARG_LIST_HPP__

#include <cstdint>
#include <arch-specific/types.hpp>

namespace OS {

class ArgList {
public:
  virtual bool PopBool() = 0;
  virtual int PopInt() = 0;
  virtual uint32_t PopUInt32() = 0;
  virtual uint64_t PopUInt64() = 0;
  virtual PhysAddr PopPhysAddr() = 0;
  virtual VirtAddr PopVirtAddr() = 0;
};

}

#endif
