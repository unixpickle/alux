#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/smp/startup-code.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <arch/general/clock.hpp>
#include <panic>
#include <critical>
#include <iostream>

extern "C" {
#include <anlock.h>
}

namespace OS {

namespace x64 {

static StartupCode * code;
static uint64_t curCpuLock OS_ALIGNED(8) = 0;
  
static void IterateApicIds(void (* func)(uint32_t));
static void StartCPU(uint32_t apicId);
static void CPUEntrance();
static void CPUMain();

void StartProcessors() {
  cout << "Starting processors..." << endl;
  
  size_t codeSize;
  StartupCode _code;
  code = &_code;
  
  IterateApicIds(StartCPU);
  
  code = NULL;
}

static void IterateApicIds(void (* func)(uint32_t)) {
  MADT * madt = MADT::GetGlobal();
  
  for (int i = 0; i < madt->GetTableCount(); i++) {
    uint8_t * table = madt->GetTable(i);
    if (table[0] == MADT::TypeLAPIC) {
      MADT::LocalAPIC * lapic = (MADT::LocalAPIC *)table;
      if (!(lapic->flags & 1)) continue;
    
      func((uint32_t)lapic->apicId);
    } else if (table[0] == MADT::TypeX2APIC) {
      MADT::LocalAPIC2 * lapic = (MADT::LocalAPIC2 *)table;
      if (!(lapic->flags & 1)) continue;
    
      func(lapic->x2apicId);
    }
  }
}

static void StartCPU(uint32_t apicId) {
  {
    ScopeCritical scope;
    if (apicId == LAPIC::GetCurrent().GetId()) return;
  }
  
  cout << "Starting CPU " << apicId << " ... ";
  
  // copy the two pointers to the startup stack
  code->UpdateStack(GlobalMap::GetGlobal().GetPML4(),
                    (uint64_t)CPUEntrance);

  // use the initial CPU count to tell if it's started later on
  int cpuCount = CPUList::GetGlobal().GetCount();

  // send the INIT IPI
  SetCritical(true);
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.ClearErrors();
  lapic.SendIPI(apicId, 0, 5, 1, 1);
  SetCritical(false);
  
  Clock::GetGlobal().Sleep(1);
  
  SetCritical(true);
  lapic.SendIPI(apicId, 0, 5, 0, 1);
  SetCritical(false);
  
  Clock::GetGlobal().Sleep(1);

  // send STARTUP IPI (and resend if needed, as it is on some AMD systems)
  for (int j = 0; j < 2; j++) {
    SetCritical(true);
    lapic.ClearErrors();
    lapic.SendIPI(apicId, code->GetStartupVector(), 6, 1, 0);
    SetCritical(false);

    // wait a maximum of 200ms before sending another IPI or failing
    for (int i = 0; i < 20; i++) {
      Clock::GetGlobal().MicroSleep(10000);
      anlock_lock(&curCpuLock);
      int newCount = CPUList::GetGlobal().GetCount();
      anlock_unlock(&curCpuLock);
      if (newCount != cpuCount) return;
    }
  }
  cerr << "[FAIL]" << endl;
}

static void CPUEntrance() {
  IDT::GetGlobal().Load();
  SetCritical(false);
  anlock_lock(&curCpuLock);
  
  // configure LAPIC and get ID
  SetCritical(true);
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.Enable();
  lapic.SetDefaults();
  uint32_t lapicId = lapic.GetId();
  SetCritical(false);
  
  CPU & cpu = CPUList::GetGlobal().AddEntry(lapicId);
  GDT::GetGlobal().Set();
  
  __asm__ volatile("ltr %%ax\n"
                   "mov %%rbx, %%rsp\n"
                   "call *%%rcx"
                   : : "a" (cpu.tssSelector),
                       "b" (cpu.dedicatedStack),
                       "c" (CPUMain));
}

static void CPUMain() {
  cout << "[OK]" << endl;
  
  anlock_unlock(&curCpuLock);
  
  while (1) {
    __asm__("hlt");
  }
}

}

}
