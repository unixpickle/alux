#include "main.hpp"
#include <anarch/x64/multiboot-region-list>
#include <anarch/x64/init>
#include <anarch/api/domain-list>
#include <anarch/api/clock-module>
#include <anarch/api/clock>
#include <anarch/api/timer>
#include <anarch/critical>
#include <anarch/stream>
#include <ansa/macros>
#include <ansa/cstring>

namespace {

uint64_t lapicHalfSecond = 0;

}

extern "C" {

void TickTockMethod();

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
  
  // load the DomainList
  anarch::DomainList::GetGlobal().Load();
  
  anarch::cout << "finished loading DomainList" << anarch::endl;
  
  anarch::ScopedCritical critical;
  anarch::Timer & timer = anarch::Thread::GetCurrent().GetTimer();
  lapicHalfSecond = timer.GetTicksPerMicro().ScaleInteger(500000);
  timer.SetTimeout(lapicHalfSecond, TickTockMethod);
  timer.WaitTimeout();
}

void TickTockMethod() {
  anarch::SetCritical(false);
  anarch::cout << "tick" << anarch::endl;
  anarch::ClockModule::GetGlobal().GetClock().MicroSleep(500000);
  anarch::cout << "tock" << anarch::endl;
  
  anarch::ScopedCritical critical;
  anarch::Timer & timer = anarch::Thread::GetCurrent().GetTimer();
  timer.SetTimeout(lapicHalfSecond, TickTockMethod);
  timer.WaitTimeout();
}

}
