#ifndef __GENERAL_PHYSICAL_ALLOCATOR__
#define __GENERAL_PHYSICAL_ALLOCATOR__

#include <arch-specific/types.hpp>
#include <module/module.hpp>

namespace OS {

class PhysicalAllocator : public Module {
public:
  static PhysicalAllocator & GetGlobal();
  
  /**
   * Allocate a certain number of bytes aligned to a certain power of two. This
   * may allocate more than you ask for depending on the platform, so you can
   * optionally get the full size through the realSize argument.
   * @param size The minimum number of bytes to allocate
   * @param align The alignment to use
   * @param realSize If non-NULL, *realSize will be set to the number of bytes
   * allocated.
   * @return 0 on failure; some address (aligned to `align`) on success.
   * @noncritical
   */
  virtual PhysAddr Alloc(size_t size, size_t align, size_t * realSize) = 0;
  
  /**
   * Free a piece of memory allocated with Alloc()
   * @noncritical
   */
  virtual void Free(PhysAddr address) = 0;
  
  virtual size_t Used() = 0; // @noncritical
  virtual size_t Available() = 0; // @noncritical
  virtual size_t Total() = 0; // @noncritical
  
};

}

#endif
