#ifndef __PLATFORM_X64_PAGE_ALLOCATOR_X64_H__
#define __PLATFORM_X64_PAGE_ALLOCATOR_X64_H__

#include <platform/memory.hpp>

namespace OS {

namespace x64 {

class PageAllocator {
public:
  virtual ~PageAllocator() {}
  
  /**
   * Returns (PhysAddr)0 on failure.
   */
  virtual PhysAddr AllocPage() = 0;
  
  /**
   * Free a page.
   */
  virtual void FreePage(PhysAddr p) = 0;
  
};

}

}

#endif
