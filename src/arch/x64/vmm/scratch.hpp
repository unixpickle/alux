#ifndef __X64_SCRATCH_HPP__
#define __X64_SCRATCH_HPP__

#include <arch/x64/pmm/page-allocator.hpp>
#include <common>

namespace OS {

namespace x64 {

class Scratch {
public:
  static const int PTCount = 1;
  static const VirtAddr StartAddr = 0x7FC0000000L;
  
  static void Initialize(PhysAddr pdpt, PageAllocator & alloc);
  static Scratch & GetGlobal();
  
  VirtAddr Alloc(PhysAddr page); // @critical
  void Reassign(VirtAddr addr, PhysAddr newAddr); // @critical
  void Free(VirtAddr addr); // @critical

protected:
  void Setup(PhysAddr pdpt, PageAllocator & alloc);
  bool GetBitmap(int idx);
  void FlipBitmap(int idx);
  
  uint64_t lock OS_ALIGNED(8);
  uint64_t bitmaps[PTCount * 8];
  uint64_t * scratchPTs[PTCount];
  
  static VirtAddr ScratchPTStart();
  static VirtAddr ScratchAddress(int cellIndex);
  static uint64_t * PageTableAddress(int cellIndex);
};

}

}

#endif
