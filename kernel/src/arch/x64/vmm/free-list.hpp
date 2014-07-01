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
    VirtAddr End();
    bool CanHold(size_t pageSize, size_t pageCount);
    bool IsAdjacentBehind(VirtAddr addr);
    bool Contains(Region & reg);
    bool IsBehind(VirtAddr addr);
    bool IsFilledBy(Region & reg);
    bool IsEndedBy(Region & reg);
    bool IsStartedBy(Region & reg);
  };
  
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
  Region * first = NULL;
  
  static Region * AllocRegion(VirtAddr, size_t);
  static void FreeRegion(Region * reg);
  
  void AllocInRegion(Region * reg, Region * last, Region & requested);
};

}

}

#endif
