#include <platform/multiprocessing.hpp>
#include <platform/interrupts.hpp>
#include "../interrupts/int-handlers-x64.hpp"
#include "../interrupts/pic-x64.hpp"
#include "../interrupts/ioapic-x64.hpp"
#include "../interrupts/lapic-x64.hpp"

namespace OS {

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
  apic.MapIRQ(0, 0x20);
  apic.MapIRQ(1, 0x21);
  apic.MapIRQ(6, 0x26);
  x64::IOAPIC::StartUsing();

  x64::InitializeLocalAPIC();
  x64::LAPIC & lapic = x64::GetLocalAPIC();

  cout << "OS::InitializeProcessors() - enabling first LAPIC..." << endl;
  lapic.SetDefaults();
  lapic.Enable();
  
  Panic("TODO: send CPU IPIs here");
}

}
