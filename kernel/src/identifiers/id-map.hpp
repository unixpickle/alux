#ifndef __ALUX_ID_MAP_HPP__
#define __ALUX_ID_MAP_HPP__

#include "id-allocator.hpp"
#include <ansa/linked-list>
#include <ansa/atomic>

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
    ++count;
    buckets[idOut % Buckets].Add(&anObj.idMapLink);
    return true;
  }
  
  /**
   * Remove an item from this list and free its ID.
   * @noncritical
   */
  void Remove(T & anObj) {
    anarch::ScopedLock scope(lock);
    --count;
    buckets[anObj.GetIdentifier() % Buckets].Remove(&anObj.idMapLink);
    FreeIdentifier(anObj.GetIdentifier());
  }
  
  /**
   * Find an item by its ID and return a pointer to it. If no object could be
   * found, NULL will be returned. The returned object will be retained.
   * @noncritical
   */
  T * Find(Identifier anId) {
    anarch::ScopedLock scope(lock);
    const ansa::LinkedList<T> & bucket = buckets[anId % Buckets];
    for (auto iter = bucket.GetStart(); iter != bucket.GetEnd(); ++iter) {
      T & item = *iter;
      if (item.GetIdentifier() == anId) {
        if (item.Retain()) return &item;
        else return NULL;
      }
    }
    return NULL;
  }
  
  int GetBucketCount() const {
    return Buckets;
  }
  
  ansa::LinkedList<T> & GetBucket(int idx) {
    assert(idx >= 0 && idx < Buckets);
    return buckets[idx];
  }
  
  int GetCount() const {
    return count;
  }
  
private:
  anarch::NoncriticalLock lock;
  ansa::LinkedList<T> buckets[Buckets];
  ansa::Atomic<int> count;
};

}

#endif
