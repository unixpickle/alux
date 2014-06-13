#include <scheduler/internal/kernel-task.hpp>
#include <critical>
#include <panic>
#include <new>

namespace OS {

static KernelTask globalTask;

void KernelTask::Initialize() {
  new(&globalTask) KernelTask();
  
  ScopeCritical scope;
  globalTask.Release();
}

KernelTask & KernelTask::GetGlobal() {
  return globalTask;
}

void KernelTask::Delete() {
  Panic("KernelTask::Delete() - cannot delete global object");
}

KernelTask::KernelTask() : Task(true) {
}

}
