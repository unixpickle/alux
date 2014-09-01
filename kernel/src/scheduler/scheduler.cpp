#include "scheduler.hpp"

namespace Alux {

Scheduler::Scheduler() : taskIdMap(0x10000) {
}

Scheduler::~Scheduler() {
}

Scheduler::TaskMap & Scheduler::GetTaskIdMap() {
  return taskIdMap;
}

}
