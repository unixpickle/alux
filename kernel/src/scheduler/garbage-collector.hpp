#ifndef __ALUX_GARBAGE_COLLECTOR_HPP__
#define __ALUX_GARBAGE_COLLECTOR_HPP__

#include "garbage-object.hpp"
#include <ansa/linked-list>
#include <anarch/lock>

namespace Alux {

class Scheduler;

class GarbageCollector {
public:
  GarbageCollector(Scheduler &);
  
  void Main(); // @noncritical
  
protected:
  friend class GarbageObject;
  
  void Add(GarbageObject &); // @ambicritical
  
private:
  Scheduler & scheduler;
  
  anarch::CriticalLock objectsLock;
  ansa::LinkedList<GarbageObject> objects;
};

}

#endif
