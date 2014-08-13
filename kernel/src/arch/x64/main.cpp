#include "main.hpp"
#include "../../scheduler/round-robin/rr-scheduler.hpp"
#include <anarch/x64/multiboot-region-list>
#include <anarch/x64/init>
#include <anarch/api/domain-list>
#include <anarch/api/clock-module>
#include <anarch/api/clock>
#include <anarch/critical>
#include <anarch/stream>
#include <ansa/macros>
#include <ansa/cstring>

namespace {

OS::RRScheduler gScheduler;

}

extern "C" {

void PrintLoopThread();

void AluxMainX64(void * mbootPtr) {
  anarch::x64::InitializeSingletons();
  
  anarch::StreamModule::GetGlobal().Load();
  anarch::cout << "AluxMainX64(" << (uint64_t)mbootPtr << ")"
    << anarch::endl;
  
  // this stack is going to be preserved throughout the runtime of the OS, so
  // it is acceptable to store the boot info here
  
  anarch::x64::MultibootRegionList regions(mbootPtr);
  
  // TODO: here, calculate the length of the kernel by loading the program
  anarch::x64::BootInfo bootInfo(regions, 0x300000);
  
  anarch::x64::SetBootInfo(&bootInfo);
  
  // load the modules we use
  anarch::DomainList::GetGlobal().Load();
  anarch::ClockModule::GetGlobal().Load();
  
  anarch::cout << "finished loading anarch modules!" << anarch::endl;
  
  new(&gScheduler) OS::RRScheduler();
  gScheduler.Init();
    
  OS::KernelTask & testTask = OS::KernelTask::New(gScheduler);
  anarch::State & testState = anarch::State::NewKernel(PrintLoopThread);
  OS::Thread & thread = OS::Thread::New(testTask, testState);
  gScheduler.Add(thread);
  
  anarch::SetCritical(true);
  gScheduler.Start();
}

void PrintLoopThread() {
  anarch::Clock & clock = anarch::ClockModule::GetGlobal().GetClock();
  uint64_t delta = clock.GetMicrosPerTick().Flip().ScaleInteger(10000);
  
  ansa::Lock dummyLock;
  while (1) {
    anarch::cout << clock.GetMicros() / 1000000 << "\r";
    dummyLock.Seize();
    gScheduler.SetTimeout(clock.GetTicks() + delta, dummyLock);
  }
}

}
