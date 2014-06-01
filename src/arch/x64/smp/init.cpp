#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/smp/startup-code.hpp>
#include <arch/x64/interrupts/ioapic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/general/failure.hpp>
#include <utilities/critical.hpp>
#include <iostream>

namespace OS {

namespace x64 {

static StartupCode * code;
  
static void IterateApicIds(void (* func)(uint32_t));
static void StartCPU(uint32_t apicId);
static void CPUEntrance();

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
    ScopeCritical critical;
    if (apicId == LAPIC::GetCurrent().GetId()) return;
  }
  
  // copy the two pointers to the startup stack
  code->UpdateStack(GlobalMap::GetGlobal().GetPML4(),
                    (uint64_t)CPUEntrance);

  cout << "StartCPU(" << apicId << ")" << endl;
  Panic("StartCPU - NYI");
}

static void CPUEntrance() {
  
}

}

}
