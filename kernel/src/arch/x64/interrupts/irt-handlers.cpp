#include <arch/x64/interrupts/irt-handlers.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/segments/gs-register.hpp>
#include <arch/x64/general/critical.hpp>
#include <arch/x64/common.hpp>
#include <memory/fault.hpp>
#include <panic>
#include <iostream>

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code,
                    uint64_t cs) {
  OS::x64::ScopeSwapGS scopeSwap(cs, vector);
  
  if (vector == 0xe) {
    uint64_t addr;
    __asm__("mov %%cr2, %0" : "=r" (addr));
    OS::HandleMemoryFault(addr, (code & 0x10) != 0, (code & 2) != 0);
    return;
  }
  
  OS::x64::IRT & table = OS::x64::IRT::GetGlobal();
  if (table[vector]) return table[vector]();

  if (vector >= 0xf0) return;
  
  OS::SetIgnoreCriticality(true);
  OS::cout << "OS::x64::InterruptCoded() - caller=" << (uintptr_t)caller
    << " vector=" << vector << " code=" << code << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptRegular(void * caller, uint64_t vector, uint64_t cs) {
  OS::x64::ScopeSwapGS scopeSwap(cs, vector);
  
  OS::x64::IRT & table = OS::x64::IRT::GetGlobal();
  if (table[vector]) return table[vector]();
  
  if (vector >= 0xf0) return;
  
  OS::SetIgnoreCriticality(true);
  OS::cout << "OS::x64::InterruptRegular() - caller=" << (uintptr_t)caller
    << " vector=" << vector << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptDummy(void * caller, uint64_t vector, uint64_t cs) {
  // send EOI to the PIC so it backs off
  OS::x64::ScopeSwapGS scopeSwap(cs, vector);
  
  (void)caller;
  if (vector > 0xf) {
    OS::x64::OutB(0xa0, 0x20);
  }
  OS::x64::OutB(0x20, 0x20);
}

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code,
                     uint64_t cs) {
  InterruptCoded(caller, vector, code, cs);
}

void _InterruptRegular(void * caller, uint64_t vector, uint64_t cs) {
  InterruptRegular(caller, vector, cs);
}

void _InterruptDummy(void * caller, uint64_t vector, uint64_t cs) {
  InterruptDummy(caller, vector, cs);
}

}
