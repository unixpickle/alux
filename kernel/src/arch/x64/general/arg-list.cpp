#include <arch/x64/general/arg-list.hpp>
#include <cassert>

namespace OS {

namespace x64 {

ArgList::ArgList(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
                 uint64_t arg5) {
  arguments[0] = arg1;
  arguments[1] = arg2;
  arguments[2] = arg3;
  arguments[3] = arg4;
  arguments[4] = arg5;
}

bool ArgList::PopBool() {
  return (bool)PopUInt64();
}

int ArgList::PopInt() {
  return (bool)PopUInt64();
}

uint32_t ArgList::PopUInt32() {
  return (uint32_t)PopUInt64();
}

uint64_t ArgList::PopUInt64() {
  assert(idx < 5);
  return arguments[idx++];
}

PhysAddr ArgList::PopPhysAddr() {
  return PopUInt64();
}

VirtAddr ArgList::PopVirtAddr() {
  return PopUInt64();
}

}

}
