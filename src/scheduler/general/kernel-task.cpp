#include <scheduler/general/kernel-task.hpp>
#include <arch/general/failure.hpp>
#include <new>

namespace OS {

static KernelTask globalTask;

void KernelTask::Initialize() {
  new(&globalTask) KernelTask();
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
