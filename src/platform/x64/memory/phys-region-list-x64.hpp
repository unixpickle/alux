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

#ifndef __PLATFORM_X64_PHYS_REGION_LIST_X64_H__
#define __PLATFORM_X64_PHYS_REGION_LIST_X64_H__

#include "../multiboot-x64.hpp"
#include <platform/memory.hpp>
#include <analloc2>
#include <cassert>

namespace OS {

namespace x64 {

const int MaximumPhysicalRegions = 8;
typedef ANAlloc::Region MemoryRegion;

class PhysRegionList {
private:
  MemoryRegion regions[MaximumPhysicalRegions];
  int regionCount;
  
  void AddRegion(MemoryRegion & region);
  
public:
  PhysRegionList() {}
  PhysRegionList(void * mbootPtr);
  MemoryRegion * GetRegions();
  int GetRegionCount();
  MemoryRegion * FindRegion(uintptr_t ptr);
  MemoryRegion * NextRegion(MemoryRegion * reg);
  
};

}

}

#endif

