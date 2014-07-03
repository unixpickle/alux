#ifndef __X64_ARG_LIST_HPP__
#define __X64_ARG_LIST_HPP__

#include <cstdint>
#include <arch-specific/types.hpp>

namespace Test {

class ArgList {
public:
  void PushBool(bool);
  void PushInt(int);
  void PushUInt32(uint32_t);
  void PushUInt64(uint64_t);
  void PushPhysAddr(PhysAddr);
  void PushVirtAddr(VirtAddr);
  
  uint64_t arguments[5];
  int idx = 0;
};

}

#endif
