#include <arch-specific/arg-list.hpp>

namespace Test {

void ArgList::PushBool(bool val) {
  PushUInt64((uint64_t)val);
}

void ArgList::PushInt(int val) {
  PushUInt64((uint64_t)val);
}

void ArgList::PushUInt32(uint32_t val) {
  PushUInt64((uint64_t)val);
}

void ArgList::PushUInt64(uint64_t val) {
  // assert(idx < 5);
  arguments[idx++] = val;
}

void ArgList::PushPhysAddr(PhysAddr addr) {
  PushUInt64((uint64_t)addr);
}

void ArgList::PushVirtAddr(VirtAddr addr) {
  PushUInt64((uint64_t)addr);
}

}
