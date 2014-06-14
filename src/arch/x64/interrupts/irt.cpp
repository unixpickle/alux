#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/interrupts/irt-handlers.hpp>
#include <arch/x64/interrupts/raw-handlers.hpp>
#include <arch/x64/interrupts/pic.hpp>
#include <new>

namespace OS {

namespace x64 {

static IRT globalTable;
static Module * deps[1];

void IRT::InitGlobal() {
  new(&globalTable) IRT();
}

IRT & IRT::GetGlobal() {
  return globalTable;
}

void IRT::Initialize() {
  routines = new Routine[0x100];
  bzero(routines, 0x800);
  
  ConfigureDummy();
  IDT::GetGlobal().Load();
  PICRemap(0xf0, 0xf8, 0xff, 0xff);
  // if there are a few interrupts waiting, we can trigger our handlers for all
  // of them and send appropriate EOIs to the PIC.
  __asm__("sti\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
          "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\ncli");
  Configure();
}

Module ** IRT::GetDependencies(size_t & count) {
  deps[0] = &IDT::GetGlobal();
  count = 1;
  return deps;
}

void IRT::ConfigureDummy() {
  IDT & globalIdt = IDT::GetGlobal();
  for (int i = 0; i < 0x100; i++) {
    globalIdt.SetEntry(i, (void *)RawDummyHandler, 0x8e);
  }
}

void IRT::Configure() {
  IDT & globalIdt = IDT::GetGlobal();
  for (int i = 0; i < 0x100; i++) {
    bool hasCode = (i == 0x8 || i == 0x11 || (i >= 0xa && i <= 0xe));
    if (hasCode) {
      globalIdt.SetEntry(i, (void *)RawCodedHandler, 0x8e);
    } else {
      globalIdt.SetEntry(i, (void *)RawNonCodedHandler, 0x8e);
    }
  }
}

IRT::Routine & IRT::operator[](uint8_t idx) {
  return routines[idx];
}

void IRT::Unset(uint8_t idx) {
  routines[idx] = NULL;
}

}

}
