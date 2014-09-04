#ifndef __ALUX_POOL_ID_MAP_HPP__
#define __ALUX_POOL_ID_MAP_HPP__

#include "id-map.hpp"
#include "pool-id-allocator.hpp"

namespace Alux {

template <class T, class MapType>
class PoolIdMap : public IdMap<T> {
public:
  PoolIdMap(Identifier upperBound) : allocator(upperBound) {
  }
  
  virtual MapType & GetMap() {
    return map;
  }
  
  virtual IdAllocator & GetAllocator() {
    return allocator;
  }
  
private:
  MapType map;
  PoolIdAllocator allocator;
};

}

#endif
