#include "kernel-task.hpp"
#include <anarch/critical>
#include <anarch/api/global-map>

namespace Alux {

KernelTask * KernelTask::New(Scheduler & s) {
  AssertNoncritical();
  KernelTask * t = new KernelTask(s);
  if (!t) return NULL;
  if (!t->Init()) {
    delete t;
    return NULL;
  }
  return t;
}

anarch::MemoryMap & KernelTask::GetMemoryMap() const {
  return anarch::GlobalMap::GetGlobal();
}

bool KernelTask::IsUserTask() const {
  return false;
}

void KernelTask::Dealloc() {
  Deinit();
  delete this;
}

KernelTask::KernelTask(Scheduler & s) : Task(s) {
}

}
