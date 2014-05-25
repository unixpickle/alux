#ifndef __X64_MAP_SETUP_HPP__
#define __X64_MAP_SETUP_HPP__

#include <specific/types.hpp>
#include <arch/x64/pmm/page-allocator.hpp>

namespace OS {

namespace x64 {

/**
 * This class must be instantiated and used only while an identity mapping is
 * present (at least for a reasonable amount of lower memory).
 */
class MapSetup {
private:
  PageAllocator & allocator;
  
  PhysAddr pdpt, pml4;
  int pdtOffset, pdptOffset;
  PhysAddr currentPDT;
  
  VirtAddr firstUnmappedVirtual;
  void MapNextVirtual();
  
public:
  MapSetup(PageAllocator & allocator);
  void Map();
  VirtAddr GetFirstUnmapped();
  PhysAddr GetPDPT();
  PhysAddr GetPML4();
  
};

}

}

#endif
