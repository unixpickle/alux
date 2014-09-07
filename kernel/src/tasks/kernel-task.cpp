#include "kernel-task.hpp"
#include <anarch/critical>
#include <anarch/api/global-map>

namespace Alux {

KernelTask * KernelTask::New(Scheduler & s) {
  AssertNoncritical();
  KernelTask * t = new KernelTask(s);
  assert(t != NULL);
  return t;
}

anarch::MemoryMap & KernelTask::GetMemoryMap() {
  return anarch::GlobalMap::GetGlobal();
}

bool KernelTask::IsUserTask() {
  return false;
}

void KernelTask::Dealloc() {
  Task::Dealloc();
  delete this;
}

KernelTask::KernelTask(Scheduler & s) : Task(0, s) {
}

}
