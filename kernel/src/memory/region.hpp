#ifndef __MEMORY_REGION_HPP__
#define __MEMORY_REGION_HPP__

#include <analloc2>

namespace OS {

class MallocRegion : public ANAlloc::Malloc<ANAlloc::BBTree> {
private:
  typedef ANAlloc::Malloc<ANAlloc::BBTree> super;
  static const size_t PageSize = 0x40;

public:
  MallocRegion(void * _start, size_t _length, size_t _used)
    : super(_start, PageSize, _used, _length) {}
  
  MallocRegion * next;
};

}

#endif
