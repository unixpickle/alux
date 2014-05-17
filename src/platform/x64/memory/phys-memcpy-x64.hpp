#ifndef __PLATFORM_X64_PHYS_MEMCPY_X64_H__
#define __PLATFORM_X64_PHYS_MEMCPY_X64_H__

#include "scope-scratch-x64.hpp"
#include <cstring>

namespace OS {

namespace x64 {

void MemcpyToPhys(PhysAddr phys, VirtAddr virt, size_t size);
void MemcpyToVirt(VirtAddr virt, PhysAddr phys, size_t size);

template <class F>
void VirtualizePhysical(PhysAddr phys, size_t size, const F & func) {
  ScopeScratch scope(phys & ~0xfffL);
  size_t offset = 0;
  while (offset < size) {
    size_t amount = 0x1000 - (phys & 0xfff);
    if (amount > size) amount = size;
    func((void *)(scope.GetVirtAddr() + (phys & 0xfff)), offset, amount);
    offset += amount;
    if (size > offset) {
      phys += amount;
      scope.Reassign(phys);
    }
  }
}

}

}

#endif

