#include "garbage-object.hpp"
#include "garbage-collector.hpp"

namespace Alux {

void GarbageObject::ThrowAway() {
  collector.Add(*this);
}

GarbageObject::GarbageObject(GarbageCollector & c)
  : garbageLink(*this), collector(c) {
}

GarbageObject::~GarbageObject() {  
}

}
