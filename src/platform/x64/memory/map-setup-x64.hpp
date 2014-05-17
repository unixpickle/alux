#ifndef __PLATFORM_X64_MAP_SETUP_X64_H__
#define __PLATFORM_X64_MAP_SETUP_X64_H__

#include <cstring>
#include "page-allocator-x64.hpp"
#include "size-config-x64.hpp"

namespace OS {

namespace x64 {

/**
 * The lifecycle of this class assumes you are in the physical address space--
 * that is, an identity mapped virtual address space. In reality, only the
 * first GB is identity mapped, but that ought to be enough.
 */
class MapSetup {
private:
  PageAllocator * allocator;
  
  PhysAddr pdpt, pml4;
  int pdtOffset, pdptOffset;
  PhysAddr currentPDT;
  
  VirtAddr firstUnmappedVirtual;
  void MapNextVirtual();
  
public:
  MapSetup(PageAllocator * allocator);
  void Map();
  VirtAddr GetFirstUnmapped();
  PhysAddr GetPDPT();
  PhysAddr GetPML4();
  
};

}

}

#endif

