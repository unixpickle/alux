#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/x64/vmm/kernel-layout.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/interrupts/errors.hpp>
#include <arch/x64/acpi/sdt.hpp>
#include <arch/x64/acpi/madt.hpp>
#include <arch/x64/smp/init.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <arch/x64/time/clock.hpp>
#include <arch/x64/time/pit.hpp>
#include <arch/x64/time/hpet.hpp>
#include <arch/x64/time/lapic.hpp>
#include <arch/x64/scheduler/tick-timer.hpp>
#include <arch/x64/general/critical.hpp>
#include <arch/x64/general/panic.hpp>
#include <arch/general/critical.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/general/init.hpp>
#include <iostream>
#include <cassert>

namespace OS {

namespace x64 {

void InitializeMemory(void * mbootPtr) {
  cout << "Initializing the memory subsystem..." << endl;
  
  RegionList::Initialize(mbootPtr);
  RegionList & regions = RegionList::GetGlobal();
  cout << "Regions:";
  for (int i = 0; i < regions.GetRegionCount(); i++) {
    MemoryRegion & reg = regions.GetRegions()[i];
    cout << " " << reg.GetStart() << ":" << reg.GetSize();
  }
  cout << endl;
  
  StepAllocator alloc(KernelSize());
  
  GlobalMap::Initialize(&alloc);
  GlobalMap & map = GlobalMap::GetGlobal();
  Scratch::Initialize(map.GetPDPT(), alloc);
  map.Set();
  
  Allocator::Initialize(alloc);
  map.allocator = &Allocator::GetGlobal();
}

void InitializeInterrupts() {
  cout << "Initializing the interrupt subsystem..." << endl;
  
  IDT::Initialize();
  IRT::Initialize();
  
  IRT::GetGlobal()[0x2] = HandleNMIReceived;
  IRT::GetGlobal()[0x12] = HandleMCEReceived;
  IDT::GetGlobal().SetIST(0x2, 1);
  IDT::GetGlobal().SetIST(0x12, 1);
  
  SetIgnoreCriticality(false);
  SetCritical(false);
}

void InitializeACPI() {
  cout << "Initializing the ACPI subsystem..." << endl;
  
  SDT::Initialize();
  MADT::Initialize();
}

void InitializeAPIC() {
  cout << "Initialize I/O APIC and Local APIC..." << endl;
  
  IOAPIC::Initialize();
  LAPIC::Initialize();
  
  SetCritical(true);
  
  IOAPIC & base = IOAPIC::GetBase();
  for (uint32_t i = 0; i < base.GetPinCount(); i++) {
    base.MaskPin((int)i);
  }
  IOAPIC::StartUsing();
  
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.Enable();
  lapic.SetDefaults();
  
  SetCritical(false);
}

void InitializeTime() {
  cout << "Initializing the time subsystem..." << endl;
  
  if (HPET::IsSupported()) {
    HPET::Initialize();
    SetCurrentClock(&HPET::GetGlobal());
    HPET::GetGlobal().Start();
  } else {
    PIT::Initialize();
    PIT::GetGlobal().SetDivisor(1193);
    SetCurrentClock(&PIT::GetGlobal());
    PIT::Start();
  }
}

void InitializeSMP() {
  cout << "Initializing SMP subsystem..." << endl;
  
  MADT * madt = MADT::GetGlobal();
  assert(madt != NULL);
  CPUList::Initialize(madt->CountLAPICs(true));
  
  // initialize GDT and TSS
  GDT::Initialize();
  GDT & gdt = GDT::GetGlobal();
  
  SetCritical(true);
  uint32_t lapicId = LAPIC::GetCurrent().GetId();
  SetCritical(false);
  
  CPU & cpu = CPUList::GetGlobal().AddEntry(lapicId);
  SetCritical(true);
  cpu.LoadGS();
  GDT::GetGlobal().Set();
  __asm__ volatile("ltr %%ax" : : "a" (cpu.GetTSSSelector()));
  SetCritical(false);
  
  StartProcessors();
  InitializeInvlpg();
  InitializePanic();
}

void InitializeTimers() {
  cout << "Initializing CPU timers..." << endl;
  
  InitializeLapicTimers();
  
  // TODO: here is where I might setup invariant TSC for a better clock
}

void InitializeScheduler() {
  cout << "Initializing scheduler..." << endl;
  IRT::GetGlobal()[IntVectors::LapicTimer] = LapicTickMethod;
  TickTimer::Initialize();
  OS::InitializeScheduler();
  Scheduler::GetGlobal().Start();
}

}

}
