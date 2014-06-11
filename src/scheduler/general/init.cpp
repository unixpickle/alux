#include <scheduler/internal/pid-pool.hpp>
#include <scheduler/internal/kernel-task.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <utilities/critical.hpp>

namespace OS {

void InitializeScheduler() {
  AssertNoncritical();
  PIDPool::Initialize();
  Scheduler::Initialize();
  KernelTask::Initialize();
  GarbageThread::Initialize(&KernelTask::GetGlobal());
  GarbageThread * thread = &GarbageThread::GetGlobal();
  
  ScopeCritical critical;
  KernelTask::GetGlobal().AddThread(thread);
  Scheduler::GetGlobal().AddThread(thread);
}

}
