#include "kernel-task.hpp"
#include <anarch/api/global-map>
#include <anarch/critical>

namespace OS {

KernelTask & KernelTask::New(Scheduler & s) {
  KernelTask & res = *(new KernelTask(s));
  res.Init();
  return res;
}

anarch::MemoryMap & KernelTask::GetMemoryMap() const {
  return anarch::GlobalMap::GetGlobal();
}

bool KernelTask::IsUserTask() const {
  return false;
}

KernelTask::KernelTask(Scheduler & s) : Task(s) {
}

void KernelTask::Destroy() {
  AssertNoncritical();
  Deinit();
  delete this;
}

}
