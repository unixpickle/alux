#include "garbage-object.hpp"
#include "garbage-thread.hpp"
#include <anarch/critical>

namespace OS {

GarbageObject::GarbageObject(GarbageCollector & c) : garbageCollector(c) {
}

void GarbageObject::ThrowAway() {
  AssertCritical();
  garbageCollector.Push(*this);
}

void GarbageObject::ThrowAwaySync() {
  AssertNoncritical();
  Destroy();
}

}
