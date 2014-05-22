#include "pmm-x64.hpp"

namespace OS {

bool PhysicalAlloc(size_t size, PhysAddr & addr, size_t * realSize) {
  AssertNoncritical();
  x64::PMM & alloc = x64::PMM::GetGlobal();
  return alloc.Alloc(size, addr, realSize);
}

bool PhysicalAlign(size_t size,
                   size_t align,
                   PhysAddr & addr,
                   size_t * realSize) {
  AssertNoncritical();
  x64::PMM & alloc = x64::PMM::GetGlobal();
  return alloc.Align(size, align, addr, realSize);
}

bool PhysicalAllocBelow(size_t size,
                        PhysAddr & addr,
                        size_t * realSize,
                        PhysAddr boundary) {
  AssertNoncritical();
  x64::PMM & alloc = x64::PMM::GetGlobal();
  return alloc.AllocBelow(size, addr, realSize, boundary);
}

bool PhysicalAlignBelow(size_t size,
                        size_t align,
                        PhysAddr & addr,
                        size_t * realSize,
                        PhysAddr boundary) {
  AssertNoncritical();
  x64::PMM & alloc = x64::PMM::GetGlobal();
  return alloc.AlignBelow(size, align, addr, realSize, boundary);
}

void PhysicalFree(PhysAddr addr) {
  AssertNoncritical();
  x64::PMM::GetGlobal().Free(addr);
}

size_t PhysicalAvailable() {
  AssertNoncritical();
  return x64::PMM::GetGlobal().Available();
}

size_t PhysicalUsed() {
  AssertNoncritical();
  return x64::PMM::GetGlobal().Used();
}

}
