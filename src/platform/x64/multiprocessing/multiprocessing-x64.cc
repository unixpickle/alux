#include "../interrupts/int-handlers-x64.hpp"
#include "../interrupts/pic-x64.hpp"
#include "../interrupts/ioapic-x64.hpp"
#include "../interrupts/lapic-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "../memory/kernel-map-x64.hpp"
#include "../gdt-x64.hpp"
#include "cpu-x64.hpp"
#include "pit-x64.hpp"

namespace OS {

namespace x64 {

static const uint16_t codeAddress = 0x5000;
static uint64_t curCpuLock OS_ALIGNED(8) = 0;

static void SetupCPUList();
static void AddThisCPU();
static void StartCPUs();
static void * CopyCPUCode(size_t & size);
static void UncopyCPUCode(void * backup, size_t size);
static void StartCPU(uint32_t lapicId);
static void CalibrateCPUs();

static void Entrance();
static void CpuMain();

void InitializeProcessors() {
  InitializeIDT();
  ConfigureDummyIDT();
  GetGlobalIDT().Load();
  cout << "OS::x64::InitializeProcessors() - disabling PIC..." << endl;
  RemapPIC(0xf0, 0xf8, 0xff, 0xff);
  SetInterruptsEnabled(true);
  // if there are a few interrupts masked, we can trigger our handlers for all
  // of them and send appropriate EOIs to the PIC.
  __asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
  SetInterruptsEnabled(false);
  ConfigureRealIDT();

  // initailize the IOAPIC and enable the PIT
  cout << "OS::x64::InitializeProcessors() - initializing I/O APIC" << endl;
  InitializeIOAPIC();
  IOAPIC & apic = GetBaseIOAPIC();
  for (uint32_t i = 0; i < apic.GetPinCount(); i++) {
    apic.MaskPin(i);
  }
  apic.MapIRQ(0, IntVectors::PIT);
  IOAPIC::StartUsing();

  cout << "OS::x64::InitializeProcessors() - enabling first LAPIC..." << endl;
  LAPIC & lapic = GetLocalAPIC();
  lapic.SetDefaults();
  lapic.Enable();

  SetupCPUList();
  AddThisCPU();
  StartCPUs();
  CalibrateCPUs();

  // TODO: see if we need the PIT to keep time
  apic.MaskIRQ(IntVectors::PIT);
}

static void SetupCPUList() {
  cout << "OS::x64::SetupCPUList()" << endl;
  int usable = ACPI::GetMADT()->CountLocalAPICEntries(true);
  CPUList::Initialize(usable);
}

static void AddThisCPU() {
  cout << "OS::x64::AddThisCPU()" << endl;
  GDT & gdt = GDT::GetGlobal();
  int idx = CPUList::ConstructEntry(GetLocalAPIC().GetId());
  TSS * tss = new TSS();
  tss->ioplBase = 0xffff;
  
  uint16_t sel = gdt.AddTSS(tss);
  gdt.Set();
  __asm__("ltr %%ax" : : "a" (sel));
  
  CPU & cpu = CPUList::GetEntry(idx);
  cpu.tssDesc = sel;
  cpu.stack = new uint8_t[0x4000];
}

static void StartCPUs() {
  cout << "OS::x64::StartCPUs()" << endl;
  PitSetDivisor(11932);
  SetIntRoutine(IntVectors::PIT, PitInterruptHandler);
  SetInterruptsEnabled(true);

  LAPIC & lapic = GetLocalAPIC();
  ACPI::MADT * madt = ACPI::GetMADT();
  
  size_t codeSize;
  void * theCopy = CopyCPUCode(codeSize);
  cout << "OS::x64::StartCPUs() - startup code size = " << codeSize << endl;

  for (int i = 0; i < madt->GetTableCount(); i++) {
    uint8_t * table = madt->GetTable(i);
    if (table[0] == ACPI::MADT::TypeLAPIC) {
      ACPI::MADT::LocalAPIC * lapic = (ACPI::MADT::LocalAPIC *)table;
      if (!(lapic->flags & 1)) continue;
    
      StartCPU((uint32_t)lapic->apicId);
    } else if (table[0] == ACPI::MADT::Typex2APIC) {
      ACPI::MADT::LocalAPIC2 * lapic
        = (ACPI::MADT::LocalAPIC2 *)table;
      if (!(lapic->flags & 1)) continue;
    
      StartCPU(lapic->x2apicId);
    }
  }
  
  UncopyCPUCode(theCopy, codeSize);
}

static void * CopyCPUCode(size_t & sizeOut) {
  const char * buffer = (const char *)0x100000;
  const char * codeStart = NULL;
  uint64_t codeLength;
  for (int i = 0; i < 0x1000; i++) {
    if (!memcmp(buffer + i, "_X64_PROC_ENTRY_", 0x10)) {
      codeStart = buffer + i + 0x18;
      memcpy(&codeLength, buffer + i + 0x10, 8);
    }
  }
  
  void * backup = new uint8_t[codeLength + 0x1000];
  memcpy(backup, (void *)(codeAddress - 0x1000), codeLength + 0x1000);
  
  memcpy((void *)codeAddress, codeStart, codeLength);
  sizeOut = (size_t)codeLength;
  return backup;
}

static void UncopyCPUCode(void * backup, size_t size) {
  memcpy((void *)(codeAddress - 0x1000), backup, size + 0x1000);
  delete (uint8_t *)backup;
}

static void StartCPU(uint32_t lapicId) {
  if (lapicId == GetLocalAPIC().GetId()) return;
  cout << "Starting CPU " << lapicId << " ... ";
  
  // copy the two buffers
  uint64_t * buffer = (uint64_t *)(codeAddress - 0x10);
  buffer[0] = (uint64_t)KernelMap::GetGlobal().GetPML4();
  buffer[1] = (uint64_t)Entrance;
  
  // send the INIT IPI
  LAPIC & lapic = GetLocalAPIC();
  lapic.ClearErrors();
  lapic.SendIPI(lapicId, 0, 5, 1, 1);
  PitSleep(1);
  lapic.SendIPI(lapicId, 0, 5, 0, 1);
  PitSleep(1);
  
  // start the CPU
  int cpuCount = CPUList::Count();
  
  uint8_t vector = (uint8_t)(codeAddress >> 12);
  for (int j = 0; j < 2; j++) {
    lapic.ClearErrors();
    lapic.SendIPI(lapicId, vector, 6, 1, 0);
  
    // wait a maximum of 200ms before sending another IPI or failing
    for (int i = 0; i < 20; i++) {
      PitSleep(1);
      anlock_lock(&curCpuLock);
      int newCount = CPUList::Count();
      anlock_unlock(&curCpuLock);
      if (newCount != cpuCount) {
        cout << "[OK]" << endl;
        return;
      }
    }
  }
  cerr << "[FAIL]" << endl;
}

static void CalibrateCPUs() {
  Panic("TODO: implement CalibrateCPUs()");
}

static void Entrance() {
  anlock_lock(&curCpuLock);
  
  GDT & gdt = GDT::GetGlobal();
  int idx = CPUList::ConstructEntry(GetLocalAPIC().GetId());
  TSS * tss = new TSS();
  tss->ioplBase = 0xffff;
  uint16_t sel = gdt.AddTSS(tss);
  gdt.Set();
  __asm__("ltr %%ax" : : "a" (sel));
  
  CPU & cpu = CPUList::GetEntry(idx);
  cpu.tssDesc = sel;
  cpu.stack = new uint8_t[0x4000];
  
  __asm__("mov %0, %%rsp\n"
          "call *%1"
          : : "r" (cpu.stack), "r" (CpuMain));
}

static void CpuMain() {
  GetGlobalIDT().Load();
  LAPIC & lapic = GetLocalAPIC();
  lapic.SetDefaults();
  lapic.Enable();
  
  anlock_unlock(&curCpuLock);
  __asm__("sti");
  
  while (1) {
    __asm__("hlt");
  }
}
  
}

void InitializeProcessors() {
  x64::InitializeProcessors();
}

}
