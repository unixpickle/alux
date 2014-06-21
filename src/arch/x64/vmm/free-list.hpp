#ifndef __X64_FREE_LIST_HPP__
#define __X64_FREE_LIST_HPP__

#include <arch-specific/types.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

class FreeList {
public:
  class Region {
  public:
    VirtAddr start;
    size_t size;
    Region * next;
    
    Region(VirtAddr, size_t);
    bool CanHold(size_t pageSize, size_t pageCount);
    bool IsAdjacentBehind(VirtAddr addr);
  };
  
  FreeList(VirtAddr start, size_t size); // @noncritical
  
  ~FreeList(); // @noncritical
  
  /**
   * Allocate a contiguous, aligned region at a specified address.
   * @noncritical
   */
  bool AllocAt(VirtAddr start, size_t pageSize, size_t pageCount);
  
  /**
   * Allocate a contiguous, aligned region of virtual memory.
   * @noncritical
   */
  VirtAddr Alloc(size_t pageSize, size_t pageCount);
  
  /**
   * Free a contiguous region of virtual memory.
   * @noncritical
   */
  void Free(VirtAddr addr, size_t pageSize, size_t pageCount);
  
protected:
  Region * first;
  
  static Region * AllocRegion(VirtAddr, size_t);
  static void FreeRegion(Region * reg);
  
  void AllocInRegion(Region * reg, Region * last, size_t chop, size_t size);
};

}

}

#endif
