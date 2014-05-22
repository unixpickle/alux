#include "irt-x64.hpp"

namespace OS {
namespace x64 {

static IRT globalTable;

void IRT::Initialize() {
  new(&globalTable) IRT();
}

IRT & IRT::GetGlobal() {
  return globalTable;
}

IRT::IRT() {
  routines = new Routine[0x100];
  bzero(routines, 0x800);
}

void IRT::ConfigureDummy() {
  IDT & globalIdt = IDT::GetGlobal();
  for (int i = 0; i < 0x100; i++) {
    globalIdt.SetHandler(i, (void *)RawDummyHandler, 0x8e);
  }
}

void IRT::Configure() {
  IDT & globalIdt = IDT::GetGlobal();
  for (int i = 0; i < 0x100; i++) {
    bool hasCode = (i == 0x8 || i == 0x11 || (i >= 0xa && i <= 0xe));
    if (hasCode) {
      globalIdt.SetHandler(i, (void *)RawCodedHandler, 0x8e);
    } else {
      globalIdt.SetHandler(i, (void *)RawNonCodedHandler, 0x8e);
    }
  }
}

IRT::Routine & IRT::operator[](uint8_t idx) {
  return routines[idx];
}

void IRT::Unset(int idx) {
  assert(idx > 0 && idx < 0x100);
  routines[idx] = NULL;
}

}
}

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code) {
  auto table = OS::x64::IRT::GetGlobal();
  if (table[vector]) return table[vector]();

  if (vector >= 0xf0) return;
  
  OS::cout << "OS::x64::InterruptCoded() - caller=" << (uintptr_t)caller
    << " vector=" << vector << " code=" << code << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptRegular(void * caller, uint64_t vector) {
  auto table = OS::x64::IRT::GetGlobal();
  if (table[vector]) return table[vector]();
  
  if (vector >= 0xf0) return;
  
  OS::cout << "OS::x64::InterruptRegular() - caller=" << (uintptr_t)caller
    << " vector=" << vector << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptDummy(void * caller, uint64_t vector) {
  // send EOI to the PIC so it backs off
  (void)caller;
  if (vector > 0xf) {
    OS::x64::OutB(0xa0, 0x20);
  }
  OS::x64::OutB(0x20, 0x20);
}

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code) {
  InterruptCoded(caller, vector, code);
}

void _InterruptRegular(void * caller, uint64_t vector) {
  InterruptRegular(caller, vector);
}

void _InterruptDummy(void * caller, uint64_t vector) {
  InterruptDummy(caller, vector);
}

}
