#include "physical-alloc-x64.hpp"

namespace OS {

namespace KernMap {

bool Map(PhysAddr phys, size_t bytes, VirtAddr & addr) {
  bool largePages = false;
  if (!(phys & 0x1fffff) && !(bytes & 0x1fffff)) {
    largePages = true;
  }
  VirtAddr res = x64::KernelMap::GetGlobal().Map(phys, bytes, largePages);
  if (!res) return false;
  addr = res;
  return true;
}

void MarkBlank(VirtAddr addr, size_t bytes) {
  x64::KernelMap::GetGlobal().ClearMap(addr, bytes);
}

void Unmap(VirtAddr addr, size_t bytes) {
  x64::KernelMap::GetGlobal().Unmap(addr, bytes);
}

void InvalidateCache(VirtAddr addr, size_t bytes, size_t pageSize) {
  (void)pageSize;
  size_t count = bytes / 0x1000;
  if (count > 0x100) { // TODO: find a better number than 0x100
    // There are enough that it's more efficient to just flush the TLB.
    __asm__("mov %%cr4, %%rax\n"
            "xor $0x80, %%rax\n"
            "mov %%rax, %%cr4\n"
            "or $0x80, %%rax\n"
            "mov %%rax, %%cr4" : : : "rax");
  } else {
    // according to http://forum.osdev.org/viewtopic.php?f=1&t=18222, MTRR's
    // are still 4K, so we might need to invlpg for each 4K
    for (size_t i = 0; i < count; i++) {
      VirtAddr theAddr = addr + (i << 12);
      __asm__("invlpg (%0)" : : "r" (theAddr));
    }
  }
}

int GetNumPageSizes() {
  return 2;
}

size_t GetPageSize(int idx) {
  size_t arr[2] = {0x1000, 0x200000};
  return arr[idx];
}

size_t GetPageAlignment(int idx) {
  size_t arr[2] = {0x1000, 0x200000};
  return arr[idx];
}

}

}
