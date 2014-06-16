#include <arch/x64/acpi/acpi-module.hpp>
#include <arch/x64/general/text-console.hpp>
#include <arch/x64/interrupts/errors.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/scheduler/tick-timer.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/time/clock-module.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <memory/malloc.hpp>
#include <entry/main.hpp>
#include <iostream>
#include <cstdint>
#include <panic>

namespace OS {

namespace x64 {

static void InitializeSingletons(void * mboot) {
  ACPIModule::InitGlobal();
  TextConsole::InitGlobal();
  InterruptErrors::InitGlobal();
  IDT::InitGlobal();
  IRT::InitGlobal();
  IOAPICModule::InitGlobal();
  LAPICModule::InitGlobal();
  Allocator::InitGlobal();
  RegionList::InitGlobal(mboot);
  TickTimer::InitGlobal();
  GDT::InitGlobal();
  CPUList::InitGlobal();
  ClockModule::InitGlobal();
  GlobalMap::InitGlobal();
  Scratch::InitGlobal();
  Scheduler::InitGlobal();
  OutStreamModule::InitGlobal();
  MainModule::InitGlobal();
  Malloc::InitGlobal();
}

}

}

extern "C" {

void MbootEntry(void * mbootPtr) {
  OS::x64::InitializeSingletons(mbootPtr);
  
  OS::OutStreamModule::GetGlobal().Load();
  OS::cout << "MbootEntry(" << (uintptr_t)mbootPtr << ")" << OS::endl;
  
  OS::MainModule::GetGlobal().Load();
  OS::MainModule::GetGlobal().Main();
  
  OS::Panic("OS::MainModule::Main() shouldn't return");
}

/**
 * Only used when compiling with GCC and not Clang.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
