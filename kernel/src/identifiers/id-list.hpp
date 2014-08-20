#ifndef __ALUX_ID_LIST_HPP__
#define __ALUX_ID_LIST_HPP__

#include "type.hpp"
#include <anarch/lock>

namespace Alux {

/**
 * A simple mechanism for allocating identifiers.
 */
class IdentifierAllocator {
public:
  /**
   * Create a new [IdentifierAllocator] which will never return an ID greater
   * than or equal to [upperBound].
   * @ambicritical
   */
  IdentifierAllocator(Identifier upperBound);
  
  /**
   * Frees any memory used by this allocator.
   * @ambicritical
   */
  ~IdentifierAllocator();
  
  /**
   * Find an unused [Identifier] from the pool and return it. This operation is
   * atomic, so you don't have to hold any locks of your own.
   * @noncritical
   */
  bool AllocIdentifier(Identifier &);
  
  /**
   * Free an identifier so it may be returned in the future. This operation is
   * atomic just like [AllocIdentifier].
   * @noncritical
   */
  void FreeIdentifier(Identifier);
  
private:
  anarch::NoncriticalLock lock;
  Identifier upperBound;
  
  Identifier currentMax = 0;
  size_t poolCount;
  size_t poolCapacity;
  Identifier * pool;
  
  void GrowPool();
  void InitializePool();
};

}

#endif
