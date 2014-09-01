#include "kernel-task.hpp"
#include <anarch/critical>
#include <anarch/api/global-map>

#include <anarch/stream> // TODO: delete this

namespace Alux {

KernelTask * KernelTask::New(Scheduler & s) {
  AssertNoncritical();
  KernelTask * t = new KernelTask(s);
  if (!t) return NULL;
  anarch::cout << "about to attempt kernel init...";
  if (!t->Init()) {
    delete t;
    return NULL;
  }
  anarch::cout << "OK\n";
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

KernelTask::KernelTask(Scheduler & s) : Task(0, s) {
}

}
