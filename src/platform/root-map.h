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

#ifndef __PLATFORM_ROOT_MAP_H__
#define __PLATFORM_ROOT_MAP_H__

#include <platform/vm.h>
#include <memory/region.h>

namespace OS {

class RootMapper {
public:
  /**
   * The number of bytes we would like to have for the kernel.
   */
  const static uintptr_t DESIRED_SIZE = 0x10000000;
  
  /**
   * This is n, where t/2^n is the number of bytes to use for the kernel and t
   * is the total number of physical bytes of memory on the system.
   */
  const static uintptr_t DESIRED_SHIFT = 2;
  
  /**
   * Implement this in your platform specific code to return a RootMapper.
   * @discussion You should create a subclass of RootMapper which in the very
   * least returns the list of physical regions on the system.
   */
  static RootMapper * GetRootMapper();
  
  /**
   * Number of bytes to use in the kernel's address space.
   */
  virtual uintptr_t KernelMemorySize();
  
  /**
   * Returns the region mapping of physical memory.
   */
  virtual MemoryRegion * PhysicalRegions() = 0;
  
  /**
   * Returns the number of physical regions.
   */
  virtual int PhysicalRegionCount() = 0;
  
  /**
   * Attempts to create the kernel's initial address mapping.
   */
  virtual bool CreateKernelMapping(void ** firstVirt, void ** firstPhys);
  
  /**
   * Returns the first free virtual address after boot.
   */
  virtual void * FirstFreeVirtual() = 0;
  
  /**
   * Returns the first free physical address after boot.
   */
  virtual void * FirstFreePhysical() = 0;
};

class LinearPhysicalAllocator : public PhysicalAllocator {
protected:
  char * curPtr;
  
  MemoryRegion * physRegs;
  int physRegCount;
  
  MemoryRegion * virtRegs;
  int virtRegCount;

public:
  LinearPhysicalAllocator(void * curPtr, RootMapper & mapper);
  
  virtual void * Allocate(uint64_t size, size_t alignment);
  virtual void * PhysicalAddress(void * virt);
  virtual void * VirtualAddress(void * phys);
};

}

#endif
