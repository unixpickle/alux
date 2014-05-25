#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/pmm/step-allocator.hpp>
#include <arch/general/failure.hpp>

namespace OS {

namespace x64 {

StepAllocator::StepAllocator(PhysAddr start) : lastAddr(start) {
}

PhysAddr StepAllocator::AllocPage() {
  return AllocSize(0x1000);
}

PhysAddr StepAllocator::AllocSize(size_t pageSize) {
  RegionList & regions = RegionList::GetGlobal();
  MemoryRegion * reg = regions.FindRegion(lastAddr);
  if (!reg) {
    if (!(reg = regions.FindRegion(lastAddr - 1))) {
      Panic("StepAllocator::AllocSize() - lastAddr out of bounds");
    }
  }
  
  if (lastAddr % pageSize) lastAddr += pageSize - (lastAddr % pageSize);
  if (lastAddr > reg->GetEnd()) lastAddr = reg->GetEnd() - 1;
  
  while (lastAddr + pageSize > reg->GetEnd()) {
    reg = regions.NextRegion(reg);
    if (!reg) {
      Panic("StepAllocator::AllocSize() - out of memory");
    }
    lastAddr = reg->GetStart();
    if (lastAddr % pageSize) lastAddr += pageSize - (lastAddr % pageSize);
    if (lastAddr > reg->GetEnd()) lastAddr = reg->GetEnd() - 1;
  }
  
  PhysAddr res = lastAddr;
  lastAddr += pageSize;
  return res;
}

void StepAllocator::FreePage(PhysAddr) {
  Panic("StepAllocator::FreePage() - nothing to do");
}

}

}
