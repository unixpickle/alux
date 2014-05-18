#include "../interrupts/int-handlers-x64.hpp"
#include "../interrupts/pic-x64.hpp"
#include "../interrupts/ioapic-x64.hpp"
#include "../interrupts/lapic-x64.hpp"
#include "../interrupts/int-vectors-x64.hpp"
#include "cpu-x64.hpp"
#include "pit-x64.hpp"

namespace OS {

static void SetupCPUList();
static void StartCPUs();
static void StartCPU(uint32_t lapicId);
static void CalibrateCPUs();

void InitializeProcessors() {
  x64::InitializeIDT();
  x64::ConfigureDummyIDT();
  x64::GetGlobalIDT().Load();
  cout << "OS::InitializeProcessors() - disabling PIC..." << endl;
  x64::RemapPIC(0xf0, 0xf8, 0xff, 0xff);
  SetInterruptsEnabled(true);
  // if there are a few interrupts masked, we can trigger our handlers for all
  // of them and send appropriate EOIs to the PIC.
  __asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
  SetInterruptsEnabled(false);
  x64::ConfigureRealIDT();
  
  x64::InitializeIOAPIC();
  x64::IOAPIC & apic = x64::GetBaseIOAPIC();
  for (uint32_t i = 0; i < apic.GetPinCount(); i++) {
    apic.MaskPin(i);
  }
  apic.MapIRQ(0, x64::IntVectors::PIT);
  x64::IOAPIC::StartUsing();

  x64::InitializeLocalAPIC();
  x64::LAPIC & lapic = x64::GetLocalAPIC();

  cout << "OS::InitializeProcessors() - enabling first LAPIC..." << endl;
  lapic.SetDefaults();
  lapic.Enable();
  
  SetupCPUList();
  StartCPUs();
  CalibrateCPUs();
  
  // TODO: see if we need the PIT to keep time
  apic.MaskIRQ(x64::IntVectors::PIT);
}

static void SetupCPUList() {
  int usable = x64::ACPI::GetMADT()->CountLocalAPICEntries(true);
  x64::CPUList::Initialize(usable);
}

static void StartCPUs() {
  x64::PitSetDivisor(11932);
  x64::SetIntRoutine(x64::IntVectors::PIT, x64::PitInterruptHandler);
  
  // mess with them
  SetInterruptsEnabled(true);
  while (1) {
    cout << "tick" << endl;
    x64::PitSleep(100);
    cout << "tock" << endl;
    x64::PitSleep(100);
  }
  
  x64::LAPIC & lapic = x64::GetLocalAPIC();
  x64::ACPI::MADT * madt = x64::ACPI::GetMADT();
  
  for (int i = 0; i < madt->GetTableCount(); i++) {
    uint8_t * table = madt->GetTable(i);
    if (table[0] == x64::ACPI::MADT::TypeLAPIC) {
      x64::ACPI::MADT::LocalAPIC * lapic = (x64::ACPI::MADT::LocalAPIC *)table;
      if (!(lapic->flags & 1)) continue;
      
      StartCPU((uint32_t)lapic->apicId);
    } else if (table[0] == x64::ACPI::MADT::Typex2APIC) {
      x64::ACPI::MADT::LocalAPIC2 * lapic
        = (x64::ACPI::MADT::LocalAPIC2 *)table;
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
