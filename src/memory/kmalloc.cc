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
    if (s <= 0x200000 && s > size) {
      chunkAlignment = KernMap::GetPageAlignment(i);
    }
  }
  
  size_t realSize;
  void * reg = AllocatePermanentRegion(&realSize);
  firstRegion = new(reg) MallocRegion(reg, realSize, sizeof(MallocRegion));
}

void * Malloc(size_t size) {
  
}

void Free(void * ptr) {
  
}

/****************
 * MallocRegion *
 ****************/

MallocRegion::MallocRegion(void * _start, size_t _length, size_t used)
  : start(_start), length(_length), 
    tree(Log2Floor(length / PageSize) + 1, (uint8_t *)start + used),
    allocator(&tree) {
  Path p;
  allocator.Alloc(0, p);
  size_t realUsed = used + Tree::MemorySize(tree.Depth());
  if (realUsed % PageSize) {
    realUsed += PageSize - (realUsed % PageSize);
  }
  allocator.Reserve(p, 0, realUsed / PageSize);
  next = NULL;
}


void * MallocRegion::Allocate(size_t size) {
  if (size > (PageSize << (tree.Depth() - 2))) {
    return NULL;
  }
  
  int power = Log2Ceil(size) - PageSizeLog;
  if (power < 0) power = 0;
  
  int depth = tree.Depth() - power - 1;
  Path p = 0;
  // TODO: NYI
}

void MallocRegion::Free(void * buff) {
  // TODO: NYI
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
