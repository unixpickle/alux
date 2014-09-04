#ifndef __ALUX_ID_ALLOCATOR_HPP__
#define __ALUX_ID_ALLOCATOR_HPP__

#include "identifier.hpp"

namespace Alux {

class IdAllocator {
public:
  virtual ~IdAllocator() {}
  
  /**
   * Allocate an identifier.
   * 
   * The implementation is responsible for synchronization between [Alloc],
   * [Free], and [GetCount].
   *
   * @noncritical
   */
  virtual bool Alloc(Identifier &) = 0;
  
  /**
   * Allocate an identifier if there are currently less than [maxCount]
   * identifiers in use. This can be used to cap the usage of a certain type of
   * resource.
   *
   * The implementation is responsible for synchronization between [Alloc],
   * [Free], and [GetCount].
   *
   * @noncritical
   */
  virtual bool Alloc(Identifier &, int maxCount) = 0;
  
  /**
   * Free an identifier.
   * 
   * The implementation is responsible for synchronization between [Alloc],
   * [Free], and [GetCount].
   *
   * @noncritical
   */
  virtual void Free(Identifier) = 0;
  
  /**
   * Get the number of identifiers that are currently allocated.
   * 
   * The implementation is responsible for synchronization between [Alloc],
   * [Free], and [GetCount].
   *
   * @noncritical
   */
  virtual int GetCount() = 0;
};

}

#endif
