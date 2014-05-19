#include "int-init-x64.hpp"

namespace OS {
namespace x64 {

void InitializeInterrupts() {
  cout << "OS::x64::InitializeInterrupts()" << endl;
  InitializeIDT();
  ConfigureDummyIDT();
  GetGlobalIDT().Load();
  RemapPIC(0xf0, 0xf8, 0xff, 0xff);
  SetInterruptsEnabled(true);
  // if there are a few interrupts masked, we can trigger our handlers for all
  // of them and send appropriate EOIs to the PIC.
  __asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
  SetInterruptsEnabled(false);
  ConfigureRealIDT();
}

void ConfigureIOAPIC() {
  cout << "OS::x64::ConfigureIOAPIC()" << endl;
  InitializeIOAPIC();
  IOAPIC & apic = GetBaseIOAPIC();
  for (uint32_t i = 0; i < apic.GetPinCount(); i++) {
    apic.MaskPin(i);
  }
  IOAPIC::StartUsing();
}

void InitializeLAPIC() {
  LAPIC & lapic = GetLocalAPIC();
  lapic.SetDefaults();
  lapic.Enable();
  cout << "OS::x64::InitializeLAPIC() - enabled LAPIC (id=" << lapic.GetId()
    << ")" << endl;
}

}
}
