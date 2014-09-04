#ifndef __ALUX_STEP_ID_MAP_HPP__
#define __ALUX_STEP_ID_MAP_HPP__

#include "id-map.hpp"
#include "step-id-allocator.hpp"

template <class T, class MapType>
class StepIdMap : public IdMap<T> {
public:
  StepIdMap(Identifier upperBound) : allocator(map, upperBound) {
  }
  
  virtual MapType & GetMap() {
    return map;
  }
  
  virtual Allocator & GetAllocator() {
    return allocator;
  }
  
private:
  MapType map;
  StepIdAllocator allocator;
};

#endif
