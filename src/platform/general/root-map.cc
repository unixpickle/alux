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

#include <platform/root-map.h>

namespace OS {

uintptr_t RootMapper::KernelMemorySize() {
  // TODO: here, make sure we map less than half of the virtual address space!
  
  // calculate the physical memory size
  uintptr_t physSize = 0;
  uintptr_t regCount = PhysicalRegionCount();
  MemoryRegion * regs = PhysicalRegions();
  for (int i = 0; i < PhysicalRegionCount(); i++) {
    physSize += regs[i].GetSize();
  }
  
  // if we don't have enough memory to fill all our desires
  if (physSize < (DESIRED_SIZE << DESIRED_SHIFT)) {
    if (physSize < DESIRED_SIZE) return physSize;
    return DESIRED_SIZE;
  }
  
  return physSize << DESIRED_SHIFT;
}

bool RootMapper::CreateKernelMapping(void ** firstVirt, void ** firstPhys) {
  LinearPhysicalAllocator allocator(FirstFreeVirtual(), *this);
  void * mapData = allocator.Allocate(VirtualMapping::IPMappingSize(), 0x10);
  VirtualMapping * mapping = VirtualMapping::NewMappingIP(&allocator, mapData);
  
  // TODO: here, make maps from the initial physical memory to initial virtual
  // memory, making sure to use the largest page sizes available.
  return false;
}

/***************************
 * LinearPhysicalAllocator *
 ***************************/

LinearPhysicalAllocator::LinearPhysicalAllocator(void * curPtr,
                                                 RootMapper & mapper) {
  this->curPtr = (char *)curPtr;
  physRegs = mapper.PhysicalRegions();
  physRegCount = mapper.PhysicalRegionCount();
  virtRegs = VirtualMapping::VirtualMemoryRegions();
  virtRegCount = VirtualMapping::VirtualMemoryRegionCount();
}

void * LinearPhysicalAllocator::Allocate(uint64_t size, size_t alignment) {
  uintptr_t missing = (uintptr_t)curPtr % (uintptr_t)alignment;
  if (missing) {
    curPtr += (uintptr_t)alignment - missing;
  }
  char * result = curPtr;
  curPtr += size;
  return (void *)result;
}

void * LinearPhysicalAllocator::PhysicalAddress(void * virt) {
  // TODO: NYI
  return NULL;
}

void * LinearPhysicalAllocator::VirtualAddress(void * phys) {
  // TODO: NYI
  return NULL;
}

}
