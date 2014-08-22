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
    objectsLock.Release();
    anarch::SetCritical(false);
    if (!obj) {
      scheduler.SetGarbageTimeout();
    } else {
      obj->Dealloc();
    }
  }
}

void GarbageCollector::Add(GarbageObject & obj) {
  anarch::ScopedCritical critical;
  objectsLock.Seize();
  objects.Add(&obj.garbageLink);
  objectsLock.Release();
  scheduler.ClearGarbageTimeout();
}

}
