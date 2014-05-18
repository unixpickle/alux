#include "../interrupts/int-handlers-x64.hpp"
#include "../interrupts/pic-x64.hpp"
#include "../interrupts/ioapic-x64.hpp"
#include "../interrupts/lapic-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "../gdt-x64.hpp"
#include "cpu-x64.hpp"
#include "pit-x64.hpp"

namespace OS {

namespace x64 {

static void SetupCPUList();
static void AddThisCPU();
static void StartCPUs();
static void StartCPU(uint32_t lapicId);
static void CalibrateCPUs();

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
  CPUList::GetEntry(idx).tssDesc = sel;
  gdt.Set();
  __asm__("ltr %%ax" : : "a" (sel));
}

static void StartCPUs() {
  cout << "OS::x64::StartCPUs()" << endl;
  PitSetDivisor(11932);
  SetIntRoutine(IntVectors::PIT, PitInterruptHandler);

  // mess with them
  SetInterruptsEnabled(true);
  while (1) {
    cout << "tick" << endl;
    PitSleep(100);
    cout << "tock" << endl;
    PitSleep(100);
  }

  LAPIC & lapic = GetLocalAPIC();
  ACPI::MADT * madt = ACPI::GetMADT();

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
}

static void StartCPU(uint32_t lapicId) {
  (void)lapicId;
  Panic("TODO: implement StartCPU()");
}

static void CalibrateCPUs() {
  Panic("TODO: implement CalibrateCPUs()");
}
  
}

void InitializeProcessors() {
  x64::InitializeProcessors();
}

}
