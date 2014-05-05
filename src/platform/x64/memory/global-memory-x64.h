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

#ifndef __PLATFORM_X64_GLOBAL_MEMORY_X64_H__
#define __PLATFORM_X64_GLOBAL_MEMORY_X64_H__

#include "../multiboot-x64.h"
#include <platform/memory.h>
#include <memory/region.h>
#include <analloc2.h>
#include <cassert>

namespace OS {

struct AllocatorDescription {
  uintptr_t start;
  size_t depth;
  
  AllocatorDescription(const AllocatorDescription & desc) {
    *this = desc;
  }
  
  AllocatorDescription & operator=(const AllocatorDescription & desc) {
    physStart = desc.physStart;
    depth = desc.depth;
  }
};

class GlobalMap {
private:
  MemoryRegion regions[0x40];
  int regionCount;
  
  ANAlloc::BBTree trees[0x80];
  ANAlloc::Allocator<ANAlloc::BBTree> allocators[0x80];
  int allocatorCount;
  
  AllocatorDescription descriptions[0x80];
  int descriptionCount;
  
  void AddRegion(MemoryRegion & region);
  void AddDescription(AllocatorDescription & desc);
  
  /**
   * Returns the number of bytes used by the kernel and BIOS at the beginning
   * of the virtual address space.
   */
  uintptr_t VirtualUsed();
  
  /**
   * Generates the descriptions[] array.
   */
  void GenerateDescriptions();
  
  /**
   * Returns the number of bytes that the system must initially map in order to
   * properly setup the physical memory allocator.
   */
  size_t InitialBytes();
 
  /**
   * Returns the number of physical bytes of memory that will be required to
   * create the initial mappings.
   */
  size_t RequiredPhysical();
  
public:
  static GlobalMap & GetGlobalMap();
  static void InitializeGlobalMap(void * mbootPtr);
  
  GlobalMap() {}
  
  /**
   * Create a new GlobalMap, passing in the multiboot information pointer.
   */
  GlobalMap(void * mbootPtr);
  
  /**
   * Create the initial global mapping and generate physical allocators.
   */
  void Setup();
  
  /**
   * Returns the root page table for the pure kernel mapping.
   */
  PhysAddr GetPML4();
  
  /**
   * Returns the third level page table which should be placed at the beginning
   * of the PML4 in every task's address space.
   */
  PhysAddr GetPDPT();
  
};

}

#endif

