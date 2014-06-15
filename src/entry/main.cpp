#include <entry/main.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/internal/pid-pool.hpp>
#include <scheduler/internal/kernel-task.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <memory/malloc.hpp>
#include <critical>
#include <new>

namespace OS {

static MainModule gModule;

void MainModule::InitGlobal() {
  new(&gModule) MainModule();
}

MainModule & MainModule::GetGlobal() {
  return gModule;
}

void MainModule::Initialize() {
  AssertNoncritical();
  PIDPool::Initialize();
  Scheduler::GetGlobal().Load();
  KernelTask::Initialize();
  GarbageThread::Initialize(&KernelTask::GetGlobal());
  GarbageThread * thread = &GarbageThread::GetGlobal();
  
  ScopeCritical critical;
  KernelTask::GetGlobal().AddThread(thread);
  Scheduler::GetGlobal().AddThread(thread);
}

DepList MainModule::GetDependencies() {
  return DepList(&Scheduler::GetGlobal(), &Malloc::GetGlobal());
}

void MainModule::Main() {
  Scheduler::GetGlobal().Start();
}

}
