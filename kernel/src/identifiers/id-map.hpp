#ifndef __ALUX_ID_MAP_HPP__
#define __ALUX_ID_MAP_HPP__

#include "id-allocator.hpp"
#include <ansa/linked-list>

namespace Alux {

template <class T, int Buckets = 0x10>
class IdMap : private IdAllocator {
public:
  IdMap(Identifier upperBound) : IdAllocator(upperBound) {
  }
  
  /**
   * Add an item to this map and allocate an ID for it.
   * @noncritical
   */
  bool Add(T & anObj, Identifier & idOut) {
    anarch::ScopedLock scope(lock);
    if (!AllocIdentifier(idOut)) return false;
    buckets[idOut % Buckets].Add(&anObj.idMapLink);
  }
  
  /**
   * Remove an item from this list and free its ID.
   * @noncritical
   */
  void Remove(T & anObj) {
    anarch::ScopedLock scope(lock);
    buckets[anObj.GetIdentifier() % Buckets].Remove(&anObj.idMapLink);
    FreeIdentifier(anObj.GetIdentifier());
  }
  
  /**
   * Find an item by its ID and return a pointer to it. If no object could be
   * found, NULL will be returned.
   * @noncritical
   */
  T * Find(Identifier anId) const {
    anarch::ScopedLock scope(lock);
    ansa::LinkedList<T> & bucket = buckets[anId % Buckets];
    for (auto iter = bucket.GetStart(); iter != bucket.GetEnd(); ++iter) {
      T & item = *iter;
      if (item.GetIdentifier() == anId) {
        return &item;
      }
    }
    return NULL;
  }
  
private:
  anarch::NoncriticalLock lock;
  ansa::LinkedList<T> buckets[Buckets];
};

}

#endif
