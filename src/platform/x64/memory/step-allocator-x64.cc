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

#include "step-allocator-x64.h"

namespace OS {

namespace x64 {

StepAllocator::StepAllocator(PhysRegionList * _regions, PhysAddr addr)
  : lastAddr(addr), regions(_regions) { }

PhysAddr StepAllocator::AllocPage() {
  return AllocSize(0x1000);
}

PhysAddr StepAllocator::AllocSize(size_t pageSize) {
  // find the next place after or equal to nextPage where a 4K chunk is
  // readily available
  MemoryRegion * reg = regions->FindRegion(lastAddr);
  if (!reg) {
    if (!(reg = regions->FindRegion(lastAddr - 1))) {
      Panic("StepAllocator::AllocPage() - lastAddr out of bounds.");
    }
  }
  
  if (lastAddr % pageSize) lastAddr += pageSize - (lastAddr % pageSize);
  if (lastAddr > reg->GetEnd()) lastAddr = reg->GetEnd() - 1;
  
  while (lastAddr + pageSize > reg->GetEnd()) {
    reg = regions->NextRegion(reg);
    if (!reg) {
      Panic("StepAllocator::AllocPage() - out of ranges.");
    }
    lastAddr = reg->GetStart();
    if (lastAddr % pageSize) lastAddr += pageSize - (lastAddr % pageSize);
    if (lastAddr > reg->GetEnd()) lastAddr = reg->GetEnd() - 1;
  }
  
  PhysAddr res = lastAddr;
  lastAddr += pageSize;
  return res;
}

void StepAllocator<PageSize, T>::FreePage(PhysAddr addr) {
  (void)addr;
  Panic("StepAllocator::FreePage() - nothing to do");
}

}

}

#endif
