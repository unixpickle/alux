#ifndef __MEMORY_MALLOC_HPP__
#define __MEMORY_MALLOC_HPP__

#include <memory/region.hpp>

namespace OS {

class Malloc {
private:
  MallocRegion * first;
  size_t pageSize;
  size_t pageAlignment;
  size_t allocSize;
  
public:
  static const size_t RegionSize = 0x200000;
  
  static void Initialize();
  static Malloc & GetGlobal();
  
  Malloc();
  
  void * Alloc(size_t, bool getNew = true); // @ambicritical, unsynchronized
  void Free(void *); // @ambicritical, unsynchronized
  
protected:
  void MakeNewRegion();
};

}

#endif
