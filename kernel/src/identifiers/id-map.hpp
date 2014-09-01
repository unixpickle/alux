#ifndef __ALUX_ID_MAP_HPP__
#define __ALUX_ID_MAP_HPP__

#include "type.hpp"
#include <anarch/critical>
#include <ansa/linked-list>
#include <ansa/atomic>
#include <ansa/nocopy>

namespace Alux {

/**
 * An abstract way to map identifiers to objects. This class should be
 * subclassed to implement different forms of identifier allocation.
 */
template <class T, int Buckets>
class IdMap : ansa::NoCopy {
public:
  virtual ~IdMap() {}
  
  /**
   * Add an item to this map and allocate an ID for it.
   * @noncritical
   */
  bool Add(T & anObj, Identifier & idOut) {
    anarch::ScopedLock scope(lock);
    if (!Alloc(idOut)) return false;
    ++objectCount;
    buckets[idOut % Buckets].Add(&anObj.idMapLink);
    return true;
  }
  
  /**
   * Remove an item from this list and free its ID.
   * @noncritical
   */
  void Remove(T & anObj) {
    anarch::ScopedLock scope(lock);
    --objectCount;
    buckets[anObj.GetIdentifier() % Buckets].Remove(&anObj.idMapLink);
    Free(anObj.GetIdentifier());
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
    return objectCount;
  }
  
protected:
  /**
   * Override this method in a subclass to allocate a new identifier.
   */
  virtual bool Alloc(Identifier &) = 0;
  
  /**
   * Override this method in a subclass to free an identifier.
   */
  virtual void Free(Identifier) = 0;
  
private:
  anarch::NoncriticalLock lock;
  ansa::LinkedList<T> buckets[Buckets];
  ansa::Atomic<int> objectCount;
};

}

#endif
