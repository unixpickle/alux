#ifndef __ALUX_ID_MAP_HPP__
#define __ALUX_ID_MAP_HPP__

#include "id-list.hpp"

namespace Alux {

template <class T, int Buckets = 0x10>
class IdMap : private IdList {
public:
  IdMap(Identifier upperBound) : IdList(upperBound) {
  }
  
  /**
   * Add an item to this map and allocate an ID for it.
   * @noncritical
   */
  bool Add(T & anObj, Identifier & idOut) {
    if (!AllocIdentifier(idOut)) return false;
    buckets[idOut % Buckets].Add(&t.idMapLink);
  }
  
  /**
   * Remove an item from this list and free its ID.
   * @noncritical
   */
  void Remove(T & anObj) {
    buckets[idOut % Buckets].Remove(&t.idMapLink);
    FreeIdentifier(anObj.GetIdentifier());
  }
  
  T * Find(Identifier anId) {
    ansa::LinkedList<T> & bucket = buckets[anId];
    auto iter = bucket.GetStart();
    while (auto iter = bucket.GetStart(); iter != bucket.GetEnd(); ++iter) {
      T & item = *iterStart;
      if (item.GetIdentifier() == anId) {
        return &item;
      }
    }
    return NULL;
  }
  
private:
  ansa::LinkedList<T> buckets[Buckets];
};

}

#endif
