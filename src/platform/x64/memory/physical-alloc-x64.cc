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
#include "real-allocator-x64.h"

namespace OS {

static x64::PhysRegionList regions;
static x64::AllocatorList allocators;
static x64::KernelMap kernMap;

namespace x64 {

  static VirtAddr firstAddr = 0;
  static VirtAddr contAddr = 0;
  static bool usingLargePages = false;
  static RealAllocator realAllocator;
  
  /**
   * Grabs chunks of physical memory and maps them to virtual memory so that we
   * can use them for our bitmaps.
   */
  static bool GrabMore(StepAllocator & allocator, size_t & remaining);

  void InitializeKernAllocator(void * mbootPtr) {
    new(&regions) PhysRegionList(mbootPtr);
    new(&kernMap) KernelMap();
    
    // setup and use the kernel map
    StepAllocator stepper(&regions, KernelDataSize());
    kernMap.allocator = &stepper;
    kernMap.Setup();
    kernMap.Set();
    
    // figure out the allocator topology
    new(&allocators) AllocatorList(0x1000000, 0x1000, 0x1000,
                                   regions.GetRegions(),
                                   regions.GetRegionCount());
    allocators.GenerateDescriptions();
    
    size_t remaining = allocators.BitmapByteCount();
    if (remaining >= 0x200000) usingLargePages = true;
    if (remaining & 0xfff) {
      remaining += 0x1000 - (remaining & 0xfff);
    }
    
    // allocate the memory
    while (remaining) {
      if (!GrabMore(stepper, remaining)) {
        Panic("x64::InitializeKernAllocator() - GrabMore failed");
      }
    }
    
    Panic("cannot generate allocators");
    allocators.GenerateAllocators((uint8_t *)firstAddr);
    
    MemoryRegion reg(0, stepper.LastAddress());
    allocators.Reserve(reg);
    
    new(&realAllocator) RealAllocator();
    kernMap.allocator = &realAllocator;
  }

  static bool GrabMore(StepAllocator & allocator, size_t & remaining) {
    // figure out where we are
    PhysAddr & firstFree = allocator.LastAddress();
    
    MemoryRegion * reg = regions.FindRegion(firstFree);
    if (!reg) {
      if (!(reg = regions.FindRegion(firstFree - 1))) {
        Panic("x64::GrabMore() - firstFree out of bounds.");
      }
    }
    
    size_t reqSize = usingLargePages ? 0x200000 : 0x1000;
    
    // align firstFree or don't use this region
    if (firstFree % reqSize) {
      firstFree += reqSize - (firstFree % reqSize);
      if (firstFree > reg->GetEnd()) firstFree = reg->GetEnd();
    }
    
    // make sure there's enough space in this region
    size_t availSpace = reg->GetEnd() - firstFree;
    if (availSpace < reqSize) {
      reg = regions.NextRegion(reg);
      if (!reg) return false;
      firstFree = reg->GetStart();
      return true;
    }
    
    // figure out how many pages we can actually map
    size_t canGet = availSpace > remaining ? remaining : availSpace;
    if (canGet % reqSize) canGet -= canGet % reqSize;
    PhysAddr newAddr = firstFree;
    firstFree += canGet;
    
    // map it
    if (firstAddr) {
      kernMap.MapAt(contAddr, newAddr, canGet, usingLargePages);
      contAddr += canGet;
    } else {
      firstAddr = kernMap.Map(newAddr, canGet, usingLargePages);
      if (!firstAddr) return false;
      contAddr = firstAddr + canGet;
    }
    remaining -= canGet;
    return true;
  }

}

bool PhysicalAlloc(size_t size, PhysAddr & addr, size_t * realSize) {
  return PhysicalAlign(size, 1, addr, realSize);
}

bool PhysicalAlign(size_t size,
                   size_t align,
                   PhysAddr & addr,
                   size_t * realSize) {
  return false;
}

void PhysicalFree(PhysAddr addr) {
  Panic("PhysicalFree() - NYI");
}

}
