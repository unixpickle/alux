#include "main.hpp"
#include "../../memory/page-fault.hpp"
#include "../../scheduler/rr-scheduler.hpp"
#include <anarch/x64/multiboot-region-list>
#include <anarch/x64/init>
#include <anarch/api/domain-list>
#include <anarch/api/clock-module>
#include <anarch/api/clock>
#include <anarch/critical>
#include <anarch/stream>
#include <ansa/macros>
#include <ansa/cstring>

extern "C" {

void MyCoolPrintMethod();

void AluxMainX64(void * mbootPtr) {
  anarch::x64::InitializeSingletons();
  
  anarch::SetGlobalPageDelegate(Alux::HandlePageFault);
  
  anarch::StreamModule::GetGlobal().Load();
  anarch::cout << "AluxMainX64(" << (uint64_t)mbootPtr << ")"
    << anarch::endl;
  
  // this stack is going to be preserved throughout the runtime of the OS, so
  // it is acceptable to store the boot info here
  
  anarch::x64::MultibootRegionList regions(mbootPtr);
  anarch::x64::BootInfo bootInfo(regions, 0x300000);
  
  anarch::x64::SetBootInfo(&bootInfo);
  
  // load the modules we use
  anarch::DomainList::GetGlobal().Load();
  anarch::ClockModule::GetGlobal().Load();
  
  anarch::cout << "finished loading anarch modules!" << anarch::endl;
  
  Alux::RRScheduler scheduler;
  
  Alux::KernelTask * task = Alux::KernelTask::New(scheduler);
  assert(task != NULL);
  anarch::State & state = anarch::State::NewKernel(MyCoolPrintMethod);
  Alux::Thread * thread = Alux::Thread::New(*task, state);
  assert(thread != NULL);
  thread->Release();
  task->Unhold();
  
  scheduler.Run();
}

void MyCoolPrintMethod() {
  auto & scheduler = Alux::Thread::GetCurrent()->GetTask().GetScheduler();
  auto & clock = anarch::ClockModule::GetGlobal().GetClock();
  uint64_t delay = clock.GetMicrosPerTick().Flip().ScaleInteger(1000000);
  while (1) {
    anarch::cout << "my cool print method!" << anarch::endl;
    uint64_t next = clock.GetTicks() + delay;
    scheduler.SetTimeout(next);
  }
}

}
