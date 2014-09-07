#ifndef __ALUX_TASK_LIST_HPP__
#define __ALUX_TASK_LIST_HPP__

#include "retain-hash-map.hpp"
#include "../tasks/task.hpp"
#include <anidmap/id-maps>

namespace Alux {

class TaskList : public anidmap::PoolIdMap<Task, RetainHashMap<Task, 0x100> > {
public:
  typedef anidmap::PoolIdMap<Task, RetainHashMap<Task, 0x100> > super;
  
  TaskList() : super(0x10000) {
  }
  
  /**
   * Returns a retained task if one exists in the map.
   * @noncritical
   */
  Task * Find(anidmap::Identifier ident) {
    return GetMap().Find(ident);
  }
};

}

#endif
