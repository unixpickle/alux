#include "scheduler.hpp"

namespace Alux {

Scheduler::Scheduler() : taskIdMap(0x10000) {
}

Scheduler::~Scheduler() {
}

IdMap<Task> & Scheduler::GetTaskIdMap() {
  return taskIdMap;
}

}
