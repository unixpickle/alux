#ifndef __MEMORY_MALLOC_LINK_HPP__
#define __MEMORY_MALLOC_LINK_HPP__

#include <analloc2>

namespace OS {

class MallocLink {
public:
  typedef ANAlloc::BBTree TreeType;
  static MallocLink * Wrap(uint8_t * start, size_t length);
  
  ANAlloc::Malloc * malloc;
  MallocLink * next;
};

}

#endif
