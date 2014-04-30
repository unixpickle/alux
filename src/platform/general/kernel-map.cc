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

#include <platform/kernel-map.h>
#include <iostream>

namespace OS {

uintptr_t KernelMapper::KernelMemorySize() {  
  // calculate the physical memory size
  uintptr_t physSize = 0;
  int regCount = PhysicalRegionCount();
  MemoryRegion * regs = PhysicalRegions();
  for (int i = 0; i < regCount; i++) {
    physSize += regs[i].GetSize();
  }
  
  uintptr_t virSize = 0;
  regCount = VirtualMapping::VirtualRegionCount();
  regs = VirtualMapping::VirtualRegions();
  for (int i = 0; i < regCount; i++) {
    virSize += regs[i].GetSize();
  }
  
  // MIN(physSize, virSize)
  uintptr_t maxSize = (virSize > physSize ? physSize : virSize);
  
  // if we don't have enough memory to fill all our desires
  if (maxSize < (DESIRED_SIZE << DESIRED_SHIFT)) {
    if (maxSize < DESIRED_SIZE) return maxSize;
    return DESIRED_SIZE;
  }
  
  return maxSize << DESIRED_SHIFT;
}

bool KernelMapper::CreateKernelMapping(void ** firstVirt, void ** firstPhys) {
  LinearPhysicalAllocator allocator(FirstFreeVirtual(), *this);
  void * mapData = allocator.Allocate(VirtualMapping::IPMappingSize(), 0x10);
  VirtualMapping * mapping = VirtualMapping::NewMappingIP(&allocator, mapData);
  for (int i = 0; i < PhysicalRegionCount(); i++) {
    MemoryRegion & r = PhysicalRegions()[i];
    cout << "(" << (unsigned long long)r.GetStart()
      << "," << r.GetSize() << ")" << endl;
  }
  // TODO: here, make maps from the initial physical memory to initial virtual
  // memory, making sure to use the largest page sizes available.
  return false;
}

/***************************
 * LinearPhysicalAllocator *
 ***************************/

LinearPhysicalAllocator::LinearPhysicalAllocator(void * curPtr,
                                                 KernelMapper & mapper) {
  this->curPtr = (char *)curPtr;
  physRegs = mapper.PhysicalRegions();
  physRegCount = mapper.PhysicalRegionCount();
  virtRegs = VirtualMapping::VirtualRegions();
  virtRegCount = VirtualMapping::VirtualRegionCount();
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
  // go through the virtual regions to find the physical *offset*
  uintptr_t physOffset = 0;
  for (int i = 0; i < virtRegCount; i++) {
    if (virt < virtRegs[i].GetStart()) {
      Panic("LinearPhysicalAllocator::PhysicalAddress() - not found");
    }
    if (virt >= virtRegs[i].GetEnd()) {
      physOffset += virtRegs[i].GetSize();
    } else {
      physOffset += (uintptr_t)virt - (uintptr_t)virtRegs[i].GetStart();
      break;
    }
  }
  
  // go through the physical regions to find the physical *address*.
  for (int i = 0; i < physRegCount; i++) {
    if (physRegs[i].GetSize() > physOffset) {
      return physRegs[i].GetOffset(physOffset);
    }
    physOffset -= physRegs[i].GetSize();
  }
  
  Panic("LinearPhysicalAllocator::PhysicalAddress() - phys out of bounds");
  return NULL;
}

void * LinearPhysicalAllocator::VirtualAddress(void * phys) {
  // go through the physical regions to find the *offset* in virtual space
  uintptr_t virtOffset = 0;
  for (int i = 0; i < physRegCount; i++) {
    if (phys < physRegs[i].GetStart()) {
      Panic("LinearPhysicalAllocator::VirtualAddress() - not found");
    }
    if (phys >= physRegs[i].GetEnd()) {
      virtOffset += physRegs[i].GetSize();
    } else {
      virtOffset += (uintptr_t)phys - (uintptr_t)physRegs[i].GetStart();
      break;
    }
  }
  
  // go through the virtual regions to find the *address* in virtual space
  for (int i = 0; i < virtRegCount; i++) {
    if (virtRegs[i].GetSize() > virtOffset) {
      return virtRegs[i].GetOffset(virtOffset);
    }
    virtOffset -= physRegs[i].GetSize();
  }
  
  return NULL;
}

}
