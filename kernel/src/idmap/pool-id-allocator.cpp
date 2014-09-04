#include "pool-id-allocator.hpp"
#include <anarch/critical>

namespace Alux {

PoolIdAllocator::PoolIdAllocator(Identifier b)
  : upperBound(b), pool(new Identifier[1]) {
  AssertNoncritical();
  assert(upperBound > 0);
  assert(pool != NULL);
  pool[0] = 0;
}

PoolIdAllocator::~PoolIdAllocator() {
  AssertNoncritical();
  delete[] pool;
}

bool PoolIdAllocator::Alloc(Identifier & ident) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  
  // if there's nothing in the queue, add something
  if (!poolCount) {
    // if the counter is at the limit, we should fail
    if (currentMax == upperBound) return false;
    
    // add the next ID to the pool
    pool[poolCount++] = currentMax++;
  }
  
  // get an identifier from the end of the pool
  ident = pool[--poolCount];
  ++count;
  return true;
}

bool PoolIdAllocator::Alloc(Identifier & ident, int max) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  
  if (count >= max) return false;
  
  // if there's nothing in the queue, add something
  if (!poolCount) {
    // if the counter is at the limit, we should fail
    if (currentMax == upperBound) return false;
    
    // add the next ID to the pool
    pool[poolCount++] = currentMax++;
  }
  
  // get an identifier from the end of the pool
  ident = pool[--poolCount];
  ++count;
  return true;
}

void PoolIdAllocator::Free(Identifier ident) {
  AssertNoncritical();
  anarch::ScopedLock scope(lock);
  
  // if the pool is full, expand it
  if (poolCount == poolCapacity) {
    GrowPool();
  }
  
  pool[poolCount++] = ident;
  --count;
}

int PoolIdAllocator::GetCount() {
  return count;
}

void PoolIdAllocator::GrowPool() {
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

}
