#ifndef __PLATFORM_X64_KERNEL_MAP_X64_H__
#define __PLATFORM_X64_KERNEL_MAP_X64_H__

#include "map-setup-x64.hpp"
#include "../common-x64.hpp"
#include "table-mgr-x64.hpp"
#include <platform/failure.hpp>
#include <cassert>
#include <utilities/lock.hpp>
#include <stdlib/runtime.hpp>

namespace OS {

namespace x64 {

class KernelMap {
private:
  uint64_t * virtScratchPTs[ScratchPTCount];
  uint64_t scratchBitmaps[ScratchPTCount * 8];
  uint64_t scratchLock OS_ALIGNED(8);

  uint64_t mapLock OS_ALIGNED(8); // applies to bu and TableMgr
  
  // the "biggest unmapped" region of memory
  VirtAddr buStart;
  size_t buSize;
  TableMgr manager;

public:
  static KernelMap & GetGlobal();
  
  KernelMap();
  
  /**
   * Sets up the paging system using the current allocator
   */
  void Setup();
  
  /**
   * Sets CR3 to this map's PML4.
   */
  void Set();
  
  /**
   * Attempts to map a physical address to a virtual address. If the map fails,
   * (VirtualAddr)0 is returned.
   */
  VirtAddr Map(PhysAddr start, size_t size, bool largePages);

  /**
   * Map a physical address to a specified virtual address. If something was
   * already mapped there, this will Panic().
   */
  void MapAt(VirtAddr virt, PhysAddr start, size_t size, bool largePages);
  
  /**
   * Set some mapping to unpresent but keep it non-zero
   */
  void ClearMap(VirtAddr virt, size_t size);
  
  /**
   * Unmap a virtual region of memory.
   */
  void Unmap(VirtAddr virt, size_t size);

  /**
   * Temporarily map a 4K physical page into a virtual address. You should lock
   * yourself to this CPU since the caches on other CPUs may have stale entires
   * for this address.
   */
  VirtAddr AllocScratch(PhysAddr start);
  
  /**
   * Reuse the same scratch for a different address.
   */
  void ReassignScratch(VirtAddr addr, PhysAddr newAddr);
  
  /**
   * Release a virtual address returned by AllocScratch to be used elsewhere.
   */
  void FreeScratch(VirtAddr ptr);

protected:
  friend class TableMgr;
  friend class PhysicalAllocator;
  
  PageAllocator * allocator;
  
  bool CanFitRegion(size_t size, bool bigPages);

};

}

}

#endif

