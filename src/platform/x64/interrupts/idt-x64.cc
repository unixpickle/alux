#include "idt-x64.hpp"

namespace OS {

namespace x64 {

void InterruptTable::SetHandler(int idx, void * fn, uint8_t flags) {
  handlers[idx].function = (uint64_t)fn;
  handlers[idx].argument = (uint64_t)idx;
  entries[idx].SetOffset((uint64_t)&handlers[idx]);
  entries[idx].flags = flags;
}

void InterruptTable::Load() {
  idtPtr.limit = 0xfff; // TODO: see if this should be 0x1000
  idtPtr.virtualAddress = (uint64_t)entries;
  assert(!((uint64_t)&idtPtr & 0x7));
  assert(!((uint64_t)entries & 0x7));
  __asm__("lidt (%0)" : : "r" (&idtPtr));
}

}

}
