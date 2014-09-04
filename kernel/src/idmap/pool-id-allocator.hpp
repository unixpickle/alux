#ifndef __ALUX_POOL_ID_ALLOCATOR_HPP__
#define __ALUX_POOL_ID_ALLOCATOR_HPP__

#include "id-allocator.hpp"
#include <anarch/lock>

namespace Alux {

/**
 * Allocates an identifier in O(1) time and frees one in amortized O(1) time.
 * While this complexity seems ideal, the memory usage of an instance of this
 * allocator is proportional to the number of identifiers that have *ever*
 * been allocated at once on it.
 *
 * Suppose that this allocator were used to allocate identifiers for endpoint
 * descriptors of a task. Even if a user has a hard-limit of 200 outgoing
 * endpoints at any given time, that user could connect 200 times to each of,
 * let's say, 200 tasks on the system. This would result in a memory usage
 * proportional to 40000 bytes (probably 160000 bytes because each identifier
 * in the cache is 4 bytes).
 *
 * On the other hand, such a scenario as described above would not matter as
 * much for task IDs since the user wouldn't be able to scale the number of
 * descriptors they could allocate and would be capped at a hard-limit.
 */
class PoolIdAllocator : public IdAllocator {
public:
  /**
   * Create a new [PoolIdAllocator] which will never return an ID greater
   * than or equal to [upperBound].
   * @noncritical
   */
  PoolIdAllocator(Identifier b);
  
  /**
   * Frees any memory used by this allocator.
   * @noncritical
   */
  virtual ~PoolIdAllocator();
  
  virtual bool Alloc(Identifier & ident);
  virtual bool Alloc(Identifier & ident, int maxCount);
  virtual void Free(Identifier ident);
  virtual int GetCount();
  
private:
  anarch::NoncriticalLock lock;
  
  Identifier upperBound;
  
  Identifier currentMax = 1;
  size_t poolCount = 1;
  size_t poolCapacity = 1;
  Identifier * pool;
  
  int count = 0;
  
  void GrowPool();
};

}

#endif
