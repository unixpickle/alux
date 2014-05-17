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

#ifndef __PLATFORM_X64_PHYSICAL_ALLOC_X64_H__
#define __PLATFORM_X64_PHYSICAL_ALLOC_X64_H__

#include <platform/memory.h>
#include <analloc2>
#include "kernel-map-x64.h"
#include "step-allocator-x64.h"

namespace OS {

namespace x64 {

void InitializeKernAllocator(void * mbootPtr);
KernelMap * GetGlobalKernelMap();

class PhysicalAllocator : public PageAllocator {
public:
  static const int MaximumAllocators = 0x10;
  typedef ANAlloc::BBTree TreeType;
  typedef ANAlloc::AllocatorList<MaximumAllocators, TreeType> AllocatorList;
  
  static void Initialize(void * mbootPtr);
  static PhysicalAllocator & GetGlobal();
  
  PhysicalAllocator() {
    Panic("This is just for the nice compiler.");
  }
  
  PhysicalAllocator(void * mbootPtr);
  void Setup();
  
  bool Alloc(size_t size, PhysAddr & addr, size_t * realSize);
  bool Align(size_t size, size_t align, PhysAddr & addr, size_t * realSize);
  void Free(PhysAddr addr);
  size_t Available();
  size_t Used();
  
  virtual PhysAddr AllocPage();
  virtual void FreePage(PhysAddr p);

private:
  PhysRegionList regions;
  AllocatorList allocators;
  uint64_t lock OS_ALIGNED(8);
  size_t totalSpace = 0;
  
  VirtAddr AllocateBitmaps(StepAllocator & alloc,
                           PhysAddr & firstFree);
  void GrabSpace(bool large,
                 StepAllocator & alloc,
                 bool & hasStarted,
                 uint64_t & lastPtr,
                 uint64_t & firstPtr);

};

}

}

#endif
