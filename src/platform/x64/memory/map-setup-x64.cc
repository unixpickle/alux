#include "map-setup-x64.hpp"

namespace OS {

namespace x64 {

void MapSetup::MapNextVirtual() {
  if (pdtOffset == 0x200) {
    pdtOffset = 0;
    currentPDT = allocator->AllocPage();
    bzero((void *)currentPDT, 0x1000);
    ((uint64_t *)pdpt)[++pdptOffset] = currentPDT | 3;
  }
  ((uint64_t *)currentPDT)[pdtOffset++] = firstUnmappedVirtual | 0x183;
  firstUnmappedVirtual += 0x200000;
}

MapSetup::MapSetup(PageAllocator * _allocator) {
  allocator = _allocator;
  firstUnmappedVirtual = 0;
}

void MapSetup::Map() {
  size_t kernSize = KernelDataSize();
  
  // setup the initial state
  pml4 = allocator->AllocPage();
  pdpt = allocator->AllocPage();
  
  bzero((void *)pml4, 0x1000);
  bzero((void *)pdpt, 0x1000);
  
  uint64_t * pml4Buf = (uint64_t *)pml4;
  pml4Buf[0] = pdpt | 3;
  pdtOffset = 0x200;
  pdptOffset = -1;
  
  while (firstUnmappedVirtual < kernSize) {
    MapNextVirtual();
  }
}

VirtAddr MapSetup::GetFirstUnmapped() {
  return firstUnmappedVirtual;
}

PhysAddr MapSetup::GetPDPT() {
  return pdpt;
}

PhysAddr MapSetup::GetPML4() {
  return pml4;
}

}

}
