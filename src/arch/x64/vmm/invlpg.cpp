#include <arch/x64/vmm/invlpg.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

void Invlpg(VirtAddr start, size_t size, bool kernel) {
  AssertCritical();
  
  if (size > 0x200000L) {
    // at this point, it's more efficient to just clear all the caches
    if (kernel) {
      __asm__("mov %%cr4, %%rax\n"
              "xor $0x80, %%rax\n"
              "mov %%rax, %%cr4\n"
              "or $0x80, %%rax\n"
              "mov %%rax, %%cr4" : : : "rax", "memory");
    } else {
      __asm__("mov %%cr3, %%rax\n"
              "mov %%rax, %%cr3"
              : : : "rax", "memory");
    }
    return;
  }
  
  // invalidate one cache entry at a time
  for (VirtAddr addr = start; addr < start + size; addr += 0x1000) {
    __asm__("invlpg (%0)" : : "r" (addr) : "memory");
  }
}

}

}
