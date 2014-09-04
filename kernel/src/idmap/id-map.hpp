#ifndef __ALUX_ID_MAP_HPP__
#define __ALUX_ID_MAP_HPP__

#include "map.hpp"
#include "id-allocator.hpp"

namespace Alux {

/**
 * Objects in an [IdMap] need an additional `SetIdentifier(Identifier)` method.
 */
template <class T>
class IdMap {
public:
  virtual ~IdMap() {}
  
  virtual Map<T> & GetMap() = 0;
  virtual IdAllocator & GetAllocator() = 0;
  
  /**
   * Allocate an identifier for the specified object and add it to the map. If
   * an identifier could not be allocated, `false` is returned.
   * @noncritical
   */
  virtual bool Add(T & object) {
    Identifier ident;
    if (!GetAllocator().Alloc(ident)) {
      return false;
    }
    object.SetIdentifier(ident);
    GetMap().Add(object);
    return true;
  }
  
  /**
   * Bounded [Add] operation.
   * @noncritical
   */
  virtual bool Add(T & object, int maxCount) {
    Identifier ident;
    if (!GetAllocator().Alloc(ident, maxCount)) {
      return false;
    }
    object.SetIdentifier(ident);
    GetMap().Add(object);
    return true;
  }
  
  /**
   * Remove an object from the map and free its identifier.
   * @noncritical
   */
  virtual void Remove(T & object) {
    GetMap().Remove(object);
    GetAllocator().Free(object.GetIdentifier());
  }
};

}

#endif
