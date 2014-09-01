#ifndef __ALUX_POOL_ID_MAP_HPP__
#define __ALUX_POOL_ID_MAP_HPP__

#include "id-map.hpp"

namespace Alux {

/**
 * Allocates an identifier in O(1) time and frees an allocator in amortized
 * O(1) time. While this complexity seems ideal, the memory usage of an
 * instance of this allocator is proportional to the number of identifiers
 * that have *ever* been allocated at once on it.
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
template <class T, int Buckets>
class PoolIdMap : public IdMap<T, Buckets> {
public:
  /**
   * Create a new [PoolIdMap] which will never return an ID greater
   * than or equal to [upperBound].
   * @noncritical
   */
  PoolIdMap(Identifier b)
    : upperBound(b), pool(new Identifier[1]) {
    AssertNoncritical();
    assert(upperBound > 0);
    assert(pool != NULL);
    pool[0] = 0;
  }
  
  /**
   * Frees any memory used by this allocator.
   * @noncritical
   */
  virtual ~PoolIdMap() {
    AssertNoncritical();
    delete[] pool;
  }
  
protected:
  virtual bool Alloc(Identifier & ident) {
    AssertNoncritical();
    
    // if there's nothing in the queue, add something
    if (!poolCount) {
      // if the counter is at the limit, we should fail
      if (currentMax == upperBound) return false;
      
      // add the next ID to the pool
      pool[poolCount++] = currentMax++;
    }
    
    // get an identifier from the end of the pool
    ident = pool[--poolCount];
    return true;
  }
  
  virtual void Free(Identifier ident) {
    AssertNoncritical();
    
    // if the pool is full, expand it
    if (poolCount == poolCapacity) {
      GrowPool();
    }
    
    pool[poolCount++] = ident;
  }
  
private:
  Identifier upperBound;
  
  Identifier currentMax = 1;
  size_t poolCount = 1;
  size_t poolCapacity = 1;
  Identifier * pool;
  
  void GrowPool() {
    // make sure the pool *can* be expanded
    assert(poolCapacity < (size_t)upperBound);
    poolCapacity *= 2;
    if (poolCapacity > (size_t)upperBound) {
      poolCapacity = (size_t)upperBound;
    }
    
    // copy old IDs to new pool since we don't have realloc()
    Identifier * newIds = new Identifier[poolCapacity];
    assert(newIds != NULL);
    for (size_t i = 0; i < poolCount; ++i) {
      newIds[i] = pool[i];
    }
    
    // get rid of old pool and replace it
    delete[] pool;
    pool = newIds;
  }
};

}

#endif
