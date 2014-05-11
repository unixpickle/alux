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

#include <memory/kmalloc.h>

namespace OS {

static MallocRegion * firstRegion = NULL;
static uint64_t mallocLock OS_ALIGNED(8) = 0;
static size_t chunkAlignment = 0;

static void * AllocatePermanentRegion(size_t * realSize);
static void GenerateNewRegion();

void InitializeMalloc() {
  // calculate the proper chunk alignment to use with 2MB
  for (int i = 0; i < KernMap::GetNumPageSizes(); i++) {
    size_t s = KernMap::GetPageSize(i);
    if (s <= 0x200000) {
      chunkAlignment = KernMap::GetPageAlignment(i);
    }
  }
}

void * Malloc(size_t size) {
  ScopeLock scope(&mallocLock);
  assert(size <= 0x100000);
  
  MallocRegion * reg = firstRegion;
  while (reg) {
    void * ptr = reg->AllocBuf(size);
    if (ptr) return ptr;
    reg = reg->next;
  }
  
  GenerateNewRegion();
  return firstRegion->AllocBuf(size);
}

void Free(void * ptr) {
  MallocRegion * reg = firstRegion;
  while (reg) {
    if (reg->OwnsPointer(ptr)) {
      reg->FreeBuf(ptr);
      return;
    }
    reg = reg->next;
  }
  cerr << "attempt to free unowned pointer " << (uintptr_t)ptr << endl;
  Panic("Free(): fatal warning!");
}

/***********
 * Private *
 ***********/

static void * AllocatePermanentRegion(size_t * realSize) {
  PhysAddr addr;
  bool res = PhysicalAlign(0x200000, chunkAlignment, addr, realSize);
  assert(res);
  
  // map it in
  VirtAddr vAddr;
  bool result = KernMap::Map(addr, *realSize, vAddr);
  assert(result);
  
  // use our new chunk
  return (void *)vAddr;
}

static void GenerateNewRegion() {
  size_t realSize;
  void * reg = AllocatePermanentRegion(&realSize);
  MallocRegion * last = firstRegion;
  firstRegion = new(reg) MallocRegion(reg, realSize, sizeof(MallocRegion));
  firstRegion->next = last;
}

}
