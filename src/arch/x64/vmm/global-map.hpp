#ifndef __X64_GLOBAL_MAP_HPP__
#define __X64_GLOBAL_MAP_HPP__

#include <cstdint>
#include <specific/types.hpp>
#include <arch/x64/pmm/page-allocator.hpp>
#include <arch/x64/vmm/page-table.hpp>
#include <arch/x64/entry.hpp>

namespace OS {

namespace x64 {

class GlobalMap {
public:
  static void Initialize(PageAllocator * anAlloc);
  static GlobalMap & GetGlobal();
  
  GlobalMap();
  
  PhysAddr GetPDPT();
  
  friend void ::MbootEntry(void *);
  
protected:
  PageAllocator * allocator;
  PageTable table;
  PhysAddr pdpt;
  
  void Setup();
};

}

}

#endif
