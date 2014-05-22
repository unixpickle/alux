#include "smp-init-x64.hpp"

namespace OS {
namespace x64 {

static const uint16_t codeAddress = 0x5000;
static uint64_t curCpuLock OS_ALIGNED(8) = 0;

void InitializeSMP() {
  cout << "OS::x64::InitializeSMP()" << endl;
  SetupCpuList();
  StartCpus();
}

void SetupCpuList() {
  cout << "OS::x64::SetupCpuList()" << endl;
  int usable = ACPI::GetMADT()->CountLocalAPICEntries(true);
  CPUList::Initialize(usable);
  
  GDT & gdt = GDT::GetGlobal();
  int idx = CPUList::GetGlobal().ConstructEntry(LAPIC::GetCurrent().GetId());
  assert(!idx);
  TSS * tss = new TSS();
  tss->ioplBase = 0xffff;

  uint16_t sel = gdt.AddTSS(tss);
  gdt.Set();
  __asm__("ltr %%ax" : : "a" (sel));

  CPU & cpu = CPUList::GetGlobal()[idx];
  cpu.tssDesc = sel;
  cpu.stack = new uint8_t[0x4000];
}

void StartCpus() {
  size_t codeSize;
  void * theCopy = CopyCpuCode(codeSize);
  IterateApicIds(StartCpu);
  UncopyCpuCode(theCopy, codeSize);
}

void IterateApicIds(void (* func)(uint32_t)) {
  LAPIC & lapic = LAPIC::GetCurrent();
  ACPI::MADT * madt = ACPI::GetMADT();
  
  for (int i = 0; i < madt->GetTableCount(); i++) {
    uint8_t * table = madt->GetTable(i);
    if (table[0] == ACPI::MADT::TypeLAPIC) {
      ACPI::MADT::LocalAPIC * lapic = (ACPI::MADT::LocalAPIC *)table;
      if (!(lapic->flags & 1)) continue;
    
      func((uint32_t)lapic->apicId);
    } else if (table[0] == ACPI::MADT::Typex2APIC) {
      ACPI::MADT::LocalAPIC2 * lapic
        = (ACPI::MADT::LocalAPIC2 *)table;
      if (!(lapic->flags & 1)) continue;
    
      func(lapic->x2apicId);
    }
  }
}

void StartCpu(uint32_t lapicId) {
  if (lapicId == LAPIC::GetCurrent().GetId()) return;
  
  // copy the two pointers to the startup stack
  uint64_t * buffer = (uint64_t *)(codeAddress - 0x10);
  buffer[0] = (uint64_t)KernelMap::GetGlobal().GetPML4();
  buffer[1] = (uint64_t)CpuEntrance;

  // send the INIT IPI
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.ClearErrors();
  lapic.SendIPI(lapicId, 0, 5, 1, 1);
  PIT::GetGlobal().Sleep(1);
  lapic.SendIPI(lapicId, 0, 5, 0, 1);
  PIT::GetGlobal().Sleep(1);
  
  // start the CPU
  int cpuCount = CPUList::GetGlobal().GetCount();
  
  // send STARTUP IPI (and resend if needed, as it is on some AMD systems)
  uint8_t vector = (uint8_t)(codeAddress >> 12);
  for (int j = 0; j < 2; j++) {
    lapic.ClearErrors();
    lapic.SendIPI(lapicId, vector, 6, 1, 0);
  
    // wait a maximum of 200ms before sending another IPI or failing
    for (int i = 0; i < 20; i++) {
      PIT::GetGlobal().Sleep(1);
      anlock_lock(&curCpuLock);
      int newCount = CPUList::GetGlobal().GetCount();
      anlock_unlock(&curCpuLock);
      if (newCount != cpuCount) return;
    }
  }
  cerr << "OS::x64::StartCpu() - could not start APIC " << lapicId << endl;
}

void * CopyCpuCode(size_t & sizeOut) {
  const char * buffer = (const char *)0x100000;
  const char * codeStart = NULL;
  uint64_t codeLength;
  for (int i = 0; i < 0x1000; i++) {
    if (!memcmp(buffer + i, "_X64_PROC_ENTRY_", 0x10)) {
      codeStart = buffer + i + 0x18;
      memcpy(&codeLength, buffer + i + 0x10, 8);
    }
  }
  
  assert(codeStart != NULL);
  
  void * backup = new uint8_t[codeLength + 0x1000];
  memcpy(backup, (void *)(codeAddress - 0x1000), codeLength + 0x1000);
  
  memcpy((void *)codeAddress, codeStart, codeLength);
  sizeOut = (size_t)codeLength;
  return backup;
}

void UncopyCpuCode(void * backup, size_t size) {
  memcpy((void *)(codeAddress - 0x1000), backup, size + 0x1000);
  delete (uint8_t *)backup;
}

void CpuEntrance() {
  anlock_lock(&curCpuLock);
  
  GDT & gdt = GDT::GetGlobal();
  int idx = CPUList::GetGlobal().ConstructEntry(LAPIC::GetCurrent().GetId());
  TSS * tss = new TSS();
  tss->ioplBase = 0xffff;
  uint16_t sel = gdt.AddTSS(tss);
  gdt.Set();
  __asm__("ltr %%ax" : : "a" (sel));
  
  CPU & cpu = CPUList::GetGlobal()[idx];
  cpu.tssDesc = sel;
  cpu.stack = new uint8_t[0x4000];
  
  // TODO: maybe see if there's a way to properly return with a new stack so
  // that destructors get called as expected
  __asm__("mov %0, %%rsp\n"
          "call *%1"
          : : "r" (cpu.stack), "r" (CpuMain));
}

void CpuMain() {
  GetGlobalIDT().Load();
  InitializeLAPIC();
  anlock_unlock(&curCpuLock);
  
  SetInterruptsEnabled(true);
  while (1) {
    __asm__("hlt");
  }
}

}
}

