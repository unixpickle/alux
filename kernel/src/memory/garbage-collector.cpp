#include "garbage-collector.hpp"
#include "../scheduler/scheduler.hpp"
#include <anarch/critical>

namespace OS {

GarbageCollector::GarbageCollector(Thread & th) : thread(th) {
}

void GarbageCollector::Main() {
  AssertNoncritical();
  while (1) {
    while (1) {
      anarch::SetCritical(true);
      lock.Seize();
      GarbageObject * obj = objects.Shift();
      lock.Release();
      anarch::SetCritical(false);
      if (!obj) break;
      obj->Destroy();
    }
    
    anarch::CriticalScope critical;
    lock.Seize();
    thread.GetTask().GetScheduler().SetInfiniteTimeout(lock);
  }
}

void GarbageCollector::Push(GarbageObject & obj) {
  anarch::ScopedCritical critical;
  
  lock.Seize();
  objects.Add(&obj.garbageLink);
  thread.GetTask().GetScheduler().ClearTimeout(thread);
  lock.Release();
}

}
