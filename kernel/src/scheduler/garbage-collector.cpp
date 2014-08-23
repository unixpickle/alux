#include "garbage-collector.hpp"
#include "scheduler.hpp"
#include <anarch/critical>

namespace Alux {

GarbageCollector::GarbageCollector(Scheduler & s) : scheduler(s) {
}

void GarbageCollector::Main() {
  AssertNoncritical();
  while (1) {
    anarch::SetCritical(true);
    objectsLock.Seize();
    GarbageObject * obj = objects.Shift();
    if (!obj) {
      scheduler.SetGarbageTimeout(objectsLock);
      anarch::SetCritical(false);
    } else {
      objectsLock.Release();
      anarch::SetCritical(false);
      obj->Dealloc();
    }
  }
}

void GarbageCollector::Add(GarbageObject & obj) {
  anarch::ScopedCritical critical;
  objectsLock.Seize();
  objects.Add(&obj.garbageLink);
  scheduler.ClearGarbageTimeout();
  objectsLock.Release();
}

}
