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

#include <platform/memory.h>
#include <platform/failure.h>
#include "global-map-x64.h"

namespace OS {

namespace KernMap {

static PhysAddr freeRegionList = 0;
static uint64_t freeRegionLock OS_ALIGNED(8) = 0;

bool Map(PhysAddr phys, size_t bytes, VirtAddr & addr) {
  ScopeLock lock(&freeRegionLock);
  if (!freeRegionList) {
    // generate the list
  }
  return false;
}

void Unmap(VirtAddr addr, size_t bytes) {
  Panic("NYI");
}

void InvalidateCache(VirtAddr addr, size_t bytes) {
  Panic("NYI");
}

int GetNumPageSizes() {
  return 0;
}

size_t GetPageSize(int idx) {
  Panic("Out of bounds");
  return 0;
}

size_t GetPageAlignment(int idx) {
  Panic("Out of bounds");
  return 0;
}

}

}