#include "layout.hpp"
#include <anarch/stddef>

namespace OS {

namespace x64 {

Layout::Layout(void * loadAddr) : kernelStart(loadAddr) {
  kernelSize = *((uint32_t *)loadAddr + 8);
  programStart = (void *)((uint8_t *)loadAddr + kernelSize);
  programSize = *((uint32_t *)loadAddr + 9);
}

void Layout::AlignProgram() {
  VirtAddr addr = (VirtAddr)programStart;
  if (addr % 0x200000 == 0) return;
  addr += 0x200000 - (addr % 0x200000);
  
  // TODO: figure out how to reuse what might otherwise be 2MB of wasted
  // memory
  
  // move the program memory to be 2MB-aligned
  uint8_t * source = (uint8_t *)programStart;
  uint8_t * dest = (uint8_t *)addr;
  for (uint32_t idx = 0; idx < programSize; ++idx) {
    size_t i = (size_t)(programSize - idx - 1);
    dest[i] = source[i];
  }
  programStart = (void *)addr;
}

}

}
