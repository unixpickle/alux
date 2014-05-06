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

#include "map-creator-x64.h"

namespace OS {

void MapCreator::IncrementPhysOffset() {
  // find the region in which the current offset lies
  MemoryRegion * reg = regions->FindRegion(physOffset);
  if (!reg) {
    Panic("MapCreator::IncrementPhysOffset() - physOffset out of bounds.");
  }
  physOffset += 0x1000;
  while (physOffset + 0x1000 > reg->GetEnd()) {
    reg = regions->NextRegion(reg);
    if (!reg) {
      Panic("MapCreator::IncrementPhysOffset() - out of ranges.");
    }
    physOffset = reg->GetStart();
  }
}

void MapCreator::AddPhysOffset(size_t size) {
  MemoryRegion * reg = regions->FindRegion(physOffset);
  if (!reg) {
    Panic("MapCreator::IncrementPhysOffset() - physOffset out of bounds.");
  }
  while (physOffset + size > reg->GetEnd()) {
    physOffset += reg->GetEnd() - physOffset;
    size -= reg->GetEnd() - physOffset;

    reg = regions->NextRegion(reg);
    if (!reg) {
      Panic("MapCreator::IncrementPhysOffset() - out of ranges.");
    }
    physOffset = reg->GetStart();
  }
  physOffset += size;
}

void MapCreator::InitializeTables() {
  pdtOffset = 0;
  pdptOffset = 0;
  
  uint64_t * physPDPT, * physPML4;
  AllocatePage(&physPML4, NULL);
  AllocatePage(&physPDPT, NULL);
  AllocatePage(&physPDT, &virtPDT);
  pdpt = (PhysAddr)physPDPT;
  pml4 = (PhysAddr)physPML4;
  
  physPML4[0] = pdpt | 3;
  physPDPT[0] = (uint64_t)physPDT | 3;
}

void MapCreator::MapNextPage() {
  if (pdtOffset == 0x200) {
    pdtOffset = 0;
    pdptOffset++;
    AllocatePage(&physPDT, &virtPDT);
    VisiblePDPT()[pdptOffset] = (uint64_t)physPDT | 3;
  }
  VisiblePDT()[pdtOffset++] = virtMapped | 3;
  virtMapped += 0x200000;
}

void MapCreator::Switch() {
  hasSwitched = true;
  __asm__("mov %%cr3, %0" : : "r" (pml4));
  cout << "switched to new address space" << endl;
}

void MapCreator::AllocatePage(uint64_t ** phys, uint64_t ** virt) {
  if (virt) *virt = (uint64_t *)virtOffset;
  if (phys) *phys = (uint64_t *)physOffset;
  IncrementPhysOffset();
  bzero((void *)virtOffset, 0x1000);
  virtOffset += 0x1000;
}

uint64_t * MapCreator::VisiblePDT() {
  if (!hasSwitched) {
    return physPDT;
  }
  return virtPDT;
}

uint64_t * MapCreator::VisiblePDPT() {
  return (uint64_t *)pdpt;
}

MapCreator::MapCreator(PhysRegionList * r, AllocatorList * a) {
  regions = r;
  allocators = a;
  virtMapped = 0;
  hasSwitched = false;
}

void MapCreator::Map(uintptr_t total, uintptr_t current) {
  assert(!(total & 0x1fffff));
  assert(!(current & 0x1fffff));
  
  virtOffset = current;
  physOffset = current;
  
  InitializeTables();
  
  while (virtMapped < total) {
    
    MapNextPage();
    if (virtMapped >= virtOffset + 0x1000 && !hasSwitched) {
      Switch();
    }
    
  }
  
  if (!hasSwitched) {
    Switch();
  }
  
  cout << "mapped a total of " << virtMapped << " bytes" << endl;
  
  // create our scratch space
  uint64_t * physScratchPDT, * physScratchPT;
  uint64_t * virtScratchPDT;
  AllocatePage(&physScratchPDT, &virtScratchPDT);
  AllocatePage(&physScratchPT, &virtScratchPT);
  virtScratchPDT[0x1ff] = (uint64_t)physScratchPT | 3;
  VisiblePDPT()[0x1ff] = (uint64_t)physScratchPDT | 3;

  // mark space as used
  AddPhysOffset(allocators->BitmapByteCount());
  allocators->GenerateAllocators((uint8_t *)virtOffset);
  MemoryRegion region(0, physOffset);
  allocators->Reserve(region);
}

uint64_t * MapCreator::ScratchPageTable() {
  return virtScratchPT;
}

PhysAddr MapCreator::GetPML4() {
  return pml4;
}

PhysAddr MapCreator::GetPDPT() {
  return pdpt;
}

}

