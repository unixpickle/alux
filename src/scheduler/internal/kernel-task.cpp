#include <scheduler/internal/kernel-task.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler/internal/pid-pool.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <critical>
#include <panic>
#include <new>

namespace OS {

static KernelTask gTask;

void KernelTask::Initialize() {
  new(&gTask) KernelTask();
}

KernelTask & KernelTask::GetGlobal() {
  return gTask;
}

Task * KernelTask::GetTask() {
  return task;
}

void KernelTask::Initialize() {
  task = Task::New(true);
}

DepList GetDependencies() {
  return DepList(&Scheduler::GetGlobal(), &PIDPool::GetGlobal());
}

DepList GetSuperDependencies() {
  return DepList(&GarbageThread::GetGlobal());
}

}
