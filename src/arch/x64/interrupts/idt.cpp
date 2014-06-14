#include <arch/x64/interrupts/idt.hpp>
#include <cassert>
#include <new>

namespace OS {

namespace x64 {

static IDT globalTable;

void IDT::InitGlobal() {
  new(&globalTable) IDT();
}

IDT & IDT::GetGlobal() {
  return globalTable;
}

Module ** IDT::GetDependencies(size_t & count) {
  count = 0;
  return NULL;
}

void IDT::SetEntry(int idx, void * fn, uint8_t flags) {
  handlers[idx].function = (uint64_t)fn;
  handlers[idx].argument = (uint64_t)idx;
  entries[idx].SetOffset((uint64_t)&handlers[idx]);
  entries[idx].flags = flags;
}

void IDT::SetIST(int idx, uint8_t ist) {
  entries[idx].ist = ist;
}

void IDT::Load() {
  idtPtr.limit = 0xfff; // TODO: see if this should be 0x1000
  idtPtr.virtualAddress = (uint64_t)entries;
  assert(!((uint64_t)&idtPtr & 0x7));
  assert(!((uint64_t)entries & 0x7));
  __asm__("lidt (%0)" : : "r" (&idtPtr));
}

}

}
