#ifndef __ALUX_STEP_ID_ALLOCATOR_HPP__
#define __ALUX_STEP_ID_ALLOCATOR_HPP__

#include "id-allocator.hpp"
#include "unused-id-finder.hpp"
#include <anarch/lock>

namespace Alux {

/**
 * Allocate identifiers incrementally up to but not including a specified
 * limit. Both allocating and freeing run in amortized O(1) time.
 *
 * In the case when identifiers wrap around and the allocator needs to find a
 * new chunk of free identifiers, an allocation runs in O(n^2) time (when items
 * are stored in a hash-map).
 */
class StepIdAllocator : public IdAllocator {
public:
  /**
   * Create an instance with an [upperBound].
   * @ambicritical
   */
  StepIdAllocator(UnusedIdFinder & m, Identifier b);
  
  virtual bool Alloc(Identifier & identifier);
  virtual bool Alloc(Identifier & identifier, int max);
  virtual void Free(Identifier identifier);
  virtual int GetCount();
  
private:
  anarch::NoncriticalLock lock;
  UnusedIdFinder & map;
  Identifier upperBound;
  Identifier freeStart;
  Identifier freeEnd;
  int count = 0;
};

}

#endif
