/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "physical-alloc-x64.h"

namespace OS {

namespace KernMap {

bool Map(PhysAddr phys, size_t bytes, VirtAddr & addr) {
  bool largePages = false;
  if (!(phys & 0x1fffff) && !(bytes & 0x1fffff)) {
    largePages = true;
  }
  VirtAddr res = x64::GetGlobalKernelMap()->Map(phys, bytes, largePages);
  if (!res) return false;
  addr = res;
  return true;
}

void Unmap(VirtAddr addr, size_t bytes) {
  x64::GetGlobalKernelMap()->Unmap(addr, bytes);
}

void InvalidateCache(VirtAddr addr, size_t bytes, size_t pageSize) {
  size_t count = bytes / pageSize;
  if (count > 0x100) { // TODO: find a better number than 0x100
    // There are enough that it's more efficient to just flush the TLB.
    __asm__("mov %cr4, %rax\n"
            "xor $0x80, %rax\n"
            "mov %rax, %cr4\n"
            "or $0x80, %rax\n"
            "mov %rax, %cr4");
    // TODO: make sure this is all we have to do; AMD64 manual pg. 142
  } else {
    for (size_t i = 0; i < count; i++) {
      VirtAddr theAddr = addr + (i * pageSize);
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
