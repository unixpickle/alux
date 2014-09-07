#ifndef __ALUX_THREAD_LIST_HPP__
#define __ALUX_THREAD_LIST_HPP__

#include "retain-hash-map.hpp"
#include "../threads/thread.hpp"
#include <anidmap/id-maps>

namespace Alux {

class ThreadList
  : public anidmap::PoolIdMap<Thread, RetainHashMap<Thread, 0x10> > {
public:
  typedef anidmap::PoolIdMap<Thread, RetainHashMap<Thread, 0x10> > super;
  
  ThreadList() : super(0x10000) {
  }
  
  /**
   * Returns a retained thread if one exists in the map.
   * @noncritical
   */
  Thread * Find(anidmap::Identifier ident) {
    return GetMap().Find(ident);
  }
};

}

#endif
