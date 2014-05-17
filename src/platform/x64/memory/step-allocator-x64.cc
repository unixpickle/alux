#include "step-allocator-x64.hpp"

namespace OS {

namespace x64 {

StepAllocator::StepAllocator(PhysRegionList * _regions, PhysAddr addr)
  : lastAddr(addr), regions(_regions) { }

PhysAddr StepAllocator::AllocPage() {
  return AllocSize(0x1000);
}

PhysAddr StepAllocator::AllocSize(size_t pageSize) {
  // find the next place after or equal to nextPage where a 4K chunk is
  // readily available
  MemoryRegion * reg = regions->FindRegion(lastAddr);
  if (!reg) {
    if (!(reg = regions->FindRegion(lastAddr - 1))) {
      Panic("StepAllocator::AllocPage() - lastAddr out of bounds.");
    }
  }
  
  if (lastAddr % pageSize) lastAddr += pageSize - (lastAddr % pageSize);
  if (lastAddr > reg->GetEnd()) lastAddr = reg->GetEnd() - 1;
  
  while (lastAddr + pageSize > reg->GetEnd()) {
    reg = regions->NextRegion(reg);
    if (!reg) {
      Panic("StepAllocator::AllocPage() - out of ranges.");
    }
    lastAddr = reg->GetStart();
    if (lastAddr % pageSize) lastAddr += pageSize - (lastAddr % pageSize);
    if (lastAddr > reg->GetEnd()) lastAddr = reg->GetEnd() - 1;
  }
  
  PhysAddr res = lastAddr;
  lastAddr += pageSize;
  return res;
}

void StepAllocator::FreePage(PhysAddr addr) {
  (void)addr;
  Panic("StepAllocator::FreePage() - nothing to do");
}

}

}
