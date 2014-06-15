#include <arch/x64/time/clock-module.hpp>
#include <arch/x64/time/pit.hpp>
#include <arch/x64/time/hpet.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/general/global-map.hpp>
#include <arch/general/console.hpp>
#include <memory/malloc.hpp>
#include <iostream>
#include <critical>
#include <new>

namespace OS {

ClockModule & ClockModule::GetGlobal() {
  return x64::ClockModule::GetGlobal();
}

namespace x64 {

static ClockModule gClock;

void ClockModule::InitGlobal() {
  new(&gClock) ClockModule();
}

ClockModule & ClockModule::GetGlobal() {
  return gClock;
}

void ClockModule::Initialize() {
  AssertNoncritical();
  
  cout << "Initializing clock module..." << endl;
  if (HPET::IsSupported()) {
    cout << "Using the HPET for time" << endl;
    HPET::Initialize();
    realClock = &HPET::GetGlobal();
    HPET::GetGlobal().Start();
  } else {
    cout << "Using the PIT for time" << endl;
    PIT::Initialize();
    PIT::GetGlobal().SetDivisor(1193);
    realClock = &PIT::GetGlobal();
    PIT::Start();
  }
}

DepList ClockModule::GetDependencies() {
  return DepList(&Malloc::GetGlobal(), &GlobalMap::GetGlobal(),
                 &IOAPICModule::GetGlobal(), &LAPICModule::GetGlobal(),
                 &IRT::GetGlobal(), &Console::GetGlobal());
}

Clock & ClockModule::GetClock() {
  return *realClock;
}

}

}
