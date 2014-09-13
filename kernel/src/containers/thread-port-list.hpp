#ifndef __ALUX_THREAD_PORT_LIST_HPP__
#define __ALUX_THREAD_PORT_LIST_HPP__

#include "../threads/thread-port.hpp"
#include <anidmap/id-maps>

namespace Alux {

class ThreadPortList :
  public anidmap::StepIdMap<ThreadPort, anidmap::HashMap<ThreadPort, 0x10> > {
public:
  typedef anidmap::StepIdMap<ThreadPort, anidmap::HashMap<ThreadPort, 0x10> >
      super;
  
  ThreadPortList() : super(anidmap::IDENTIFIER_MAX) {
  }
  
  /**
   * Returns a [ThreadPort] given its identifier.
   * @noncritical
   */
  ThreadPort * Find(anidmap::Identifier ident) {
    return GetMap().Find(ident);
  }
};

}

#endif
