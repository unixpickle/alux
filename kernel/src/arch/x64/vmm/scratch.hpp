#ifndef __X64_SCRATCH_HPP__
#define __X64_SCRATCH_HPP__

#include <arch-specific/types.hpp>
#include <module/module.hpp>
#include <cstdint>
#include <macros>

namespace OS {

namespace x64 {

class Scratch : public Module {
public:
  static const int PTCount = 1;
  static const VirtAddr StartAddr = 0x7FC0000000L;
  
  static void InitGlobal();
  static Scratch & GetGlobal();
  
  VirtAddr Alloc(PhysAddr page); // @critical
  void Reassign(VirtAddr addr, PhysAddr newAddr); // @critical
  void Free(VirtAddr addr); // @critical

protected:
  bool GetBitmap(int idx);
  void FlipBitmap(int idx);
  
  uint64_t lock OS_ALIGNED(8) = 0;
  uint64_t bitmaps[PTCount * 8] = {};
  uint64_t * scratchPTs[PTCount];
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  static VirtAddr ScratchPTStart();
  static VirtAddr ScratchAddress(int cellIndex);
  static uint64_t * PageTableAddress(int cellIndex);
};

}

}

#endif
