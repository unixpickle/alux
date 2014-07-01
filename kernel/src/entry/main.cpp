#include <entry/main.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/internal/pid-pool.hpp>
#include <scheduler/internal/kernel-task.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <memory/malloc.hpp>
#include <iostream>
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
  cout << "MainModule::Initialize()" << endl;
  // for now, there is nothing I can see needing to do here
}

DepList MainModule::GetDependencies() {
  return DepList(&Malloc::GetGlobal(), &KernelTask::GetGlobal());
}

void MainModule::Main() {
  Scheduler::GetGlobal().Start();
}

}
