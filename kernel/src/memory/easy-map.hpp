#ifndef __MEMORY_EASY_MAP_HPP__
#define __MEMORY_EASY_MAP_HPP__

#include <arch-specific/types.hpp>
#include <cstddef>

namespace OS {

/**
 * This class makes it easy to map arbitrary misaligned regions of physical
 * memory to virtual memory. Additionally, it can provide a simple scoping
 * mechanism for memory mapping (if you're into that kinda thing).
 */
class EasyMap {
private:
  VirtAddr mapStart;
  size_t pageSize;
  size_t pageAlign;
  size_t pageCount;
  
  VirtAddr start;
  
public:
  /**
   * @noncritical
   */
  EasyMap(PhysAddr start, size_t size, bool exec = false);
  
  /**
   * @noncritical
   */
  ~EasyMap();
  
  /**
   * @ambicritical
   */
  VirtAddr GetStart() const;
};

}

#endif
