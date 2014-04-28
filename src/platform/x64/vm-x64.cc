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

#include "vm-x64.h"

#define PHYS_ADDR(x) (void *)((uintptr_t)x & 0x7FFFFFFFFFFFF000)

namespace OS {

static uint64_t mappingLock OS_ALIGNED(8) = 0;
static bool hasPageInfo = false;
static uint64_t numPageSizes;
static uintptr_t pageSizes[3];

static MemoryRegion regions[2];
static uint64_t regionsLock OS_ALIGNED(8) = 0;
static bool regionsInitialized = false;

static void _calculate_page_sizes() {
  // here, use CPUID to find page sizes etc.
  uint64_t edx1, edx2;
  __asm__("cpuid" : "=d" (edx1) : "a" (1) : "ecx");
  __asm__("cpuid" : "=d" (edx2) : "a" (0x80000001) : "ecx");
  
  numPageSizes = 1;
  pageSizes[0] = 0x1000;
  
  if (edx1 & (1 << 3)) {
    pageSizes[numPageSizes++] = 0x200000;
  }
  if (edx2 & (1 << 26)) {
    pageSizes[numPageSizes++] = 0x40000000;
  }
  
  hasPageInfo = true;
}

/*********************************
 * Static VirtualMapping Methods *
 *********************************/

int VirtualMapping::NumPageSizes() {
  ScopeLock scope(&mappingLock);
  if (!hasPageInfo) _calculate_page_sizes();
  return numPageSizes;
}

uintptr_t * VirtualMapping::PageSizes() {
  ScopeLock scope(&mappingLock);
  if (!hasPageInfo) _calculate_page_sizes();
  return pageSizes;
}

uintptr_t * VirtualMapping::PhysicalAlignments() {
  return PageSizes();
}

uintptr_t * VirtualMapping::VirtualAlignments() {
  return PageSizes();
}

void * VirtualMapping::AddPtr(void * ptr1, uintptr_t val) {
  uintptr_t sum = (uintptr_t)ptr1 + val;
  if (sum & (1L << 47)) {
    sum |= 0xFFFFL << 48;
  }
  return (void *)sum;
}

MemoryRegion * VirtualMapping::VirtualMemoryRegions() {
  ScopeLock scope(&regionsLock);
  if (regionsInitialized) return regions;
  
  new(&regions[0]) MemoryRegion(NULL, 1L << 47);
  new(&regions[1]) MemoryRegion((void *)0xFFFF800000000000L, 1L << 47);
  regionsInitialized = true;
  return regions;
}

int VirtualMapping::VirtualMemoryRegionCount() {
  return 2;
}

VirtualMapping * VirtualMapping::NewMappingIP(PhysicalAllocator * allocator,
                                              void * base) {
  StandaloneMapping * sa = new(base) StandaloneMapping(allocator);
  return static_cast<VirtualMapping *>(sa);
}

intptr_t VirtualMapping::IPMappingSize() {
  return sizeof(StandaloneMapping);
}

VirtualMapping * VirtualMapping::NewSubmapping(PhysicalAllocator * allocator,
                                               VirtualMapping * mapping) {
  StandaloneMapping * sa = static_cast<StandaloneMapping *>(mapping);
  ReferenceMapping * map = new ReferenceMapping(allocator, sa);
  return static_cast<VirtualMapping *>(map);
}

/*********************
 * StandaloneMapping *
 *********************/

void StandaloneMapping::FreeTable(uintptr_t * table, int depth) {
  if (depth == 3) {
    allocator->Free(table);
    return;
  }
  
  for (int i = 0; i < 0x200; i++) {
    if ((table[i] & 1) && !(table[i] & (1 << 7))) {
      void * physAddr = PHYS_ADDR(table[i]);
      uintptr_t * next = (uintptr_t *)allocator->VirtualAddress(physAddr);
      FreeTable(next, depth + 1);
    }
  }
  
  allocator->Free(table);
}

StandaloneMapping::StandaloneMapping(PhysicalAllocator * alloc) {
  allocator = alloc;
  pml4 = (uintptr_t *)alloc->Allocate(0x1000, 0x1000);
}

StandaloneMapping::~StandaloneMapping() {
  FreeTable(pml4, 0);
}

void StandaloneMapping::SetAllocator(PhysicalAllocator * alloc) {
  allocator = alloc;
}

void StandaloneMapping::Unmap(void * address) {
  uintptr_t pageIdx = (uintptr_t)address >> 12;
  uintptr_t * table = pml4;
  
  bool wasVacated[4];
  uintptr_t * tables[4] = {0};
  
  uintptr_t i;
  for (i = 0; i < 4; i++) {
    // find the entry in the table
    uintptr_t index = (pageIdx >> (27 - (i * 9))) & 0x1ff;
    uintptr_t entry = table[index];
    
    uintptr_t j;
    wasVacated[i] = true;
    tables[i] = table;
    for (j = 0; j < 0x200; j++) {
      if (j == index) continue;
      if (table[j]) {
        wasVacated[i] = false;
        break;
      }
    }
    
    if ((entry | (1 << 7)) || i == 3) {
      // this is the deepest entry
      table[index] = 0;
      break;
    }
    
    void * physAddr = PHYS_ADDR(entry);
    table = (uintptr_t *)allocator->VirtualAddress(physAddr);
  }
  
  // `i` is set to the deepest table we went to
  for (; i > 0; i--) {
    if (!wasVacated[i]) break;
    // free this table
    int lastIndex = (pageIdx >> (27 - ((i - 1) * 9))) & 0x1ff;
    allocator->Free(tables[i]);
    tables[i - 1][lastIndex] = 0;
  }
}

bool StandaloneMapping::Map(void * address,
                            void * phys,
                            uint64_t size,
                            int flags) {
  if ((uint64_t)address & (size - 1)) return false;
  if ((uint64_t)phys & (size - 1)) return false;
  
  // calculate the number of levels of page table to use for this size
  int levelCount = 4;
  while (((uint64_t)0x1000 << (9 * (4 - levelCount))) < size) {
    levelCount--;
  }
  if (levelCount <= 1) return false;
  
  // figure out the flags we need
  uintptr_t parentFlags = 3;
  uintptr_t childFlags = 3;
  if (flags & PageFlagUser) {
    parentFlags |= 4;
    childFlags |= 4;
  }
  if (flags & PageFlagGlobal) {
    childFlags |= 0x100;
  }
  if (!(flags & PageFlagExecute)) {
    childFlags |= (1L << 63);
  }
  
  // map each level
  uintptr_t pageIdx = (uintptr_t)address << 12;
  uintptr_t * table = pml4;
  int i;
  for (i = 0; i < levelCount - 1; i++) {
    uintptr_t index = (pageIdx >> (27 - (i * 9))) & 0x1ff;
    uintptr_t entry = table[index];
    
    if (!(entry & 1)) {
      uintptr_t * nextTable = (uintptr_t *)allocator->Allocate(0x1000, 0x1000);
      entry = (uintptr_t)allocator->PhysicalAddress(nextTable);
      entry |= parentFlags;
      table[index] = entry;
      table = nextTable;
    } else {
      table[index] |= childFlags;
      void * physAddr = PHYS_ADDR(entry);
      table = (uintptr_t *)allocator->VirtualAddress(physAddr);
    }
  }
  
  uintptr_t thisIndex = (pageIdx >> (27L - (i * 9))) & 0x1ffL;
  table[thisIndex] = childFlags | (uintptr_t)phys;
  return true;
}

bool StandaloneMapping::Lookup(void * address,
                               void ** phys,
                               void ** start,
                               int * flags,
                               uint64_t * size) {
  // here, use the typical x64 page table walk
  uintptr_t pageIdx = (uintptr_t)address >> 12;
  uintptr_t * table = pml4;
  
  uintptr_t i;
  uintptr_t offset;
  uintptr_t pointerValue = 0;
  
  for (i = 0; i < 4; i++) {
    offset = (pageIdx >> (27L - (i * 9))) & 0x1ffL;
    if (!(table[offset] & 1)) {
      return false;
    }
    pointerValue += offset << (12L + 27L - (i * 9));
    if (i == 3 || (table[i] & 0x80)) {
      break;
    }
    void * physAddr = PHYS_ADDR(table[offset]);
    table = (uintptr_t *)allocator->VirtualAddress(physAddr);
  }
  
  void * physicalStart = PHYS_ADDR(table[offset]);

  // get a canonical address
  void * virtualStart = VirtualMapping::AddPtr((void *)pointerValue, 0);
  
  // copy out the arguments
  if (size) {
    *size = (0x1000 << ((3 - i) * 9));
  }
  if (flags) {
    int vals = PageFlagExecute;
    if (table[offset] & 4) {
      vals |= PageFlagUser;
    }
    if (table[offset] & 0x100) {
      vals |= PageFlagGlobal;
    }
    if (table[offset] & (1L << 63)) {
      vals ^= PageFlagExecute;
    }
    *flags = vals;
  }
  if (phys) {
    uintptr_t difference = (uintptr_t)address - (uintptr_t)virtualStart;
    *phys = VirtualMapping::AddPtr(physicalStart, difference);
  }
  if (start) {
    *start = virtualStart;
  }
  
  return true;
}

void StandaloneMapping::MakeCurrent() {
  void * physPML4 = allocator->PhysicalAddress(pml4);
  __asm__("mov %%rax, %%cr3" : : "a" (physPML4));
}

/********************
 * ReferenceMapping *
 ********************/

bool ReferenceMapping::IsReferenced(int idx) {
  int bitIndex = idx & 0x40;
  int cellIndex = idx >> 6;
  return (refs[cellIndex] & (1L << bitIndex)) != 0;
}

void ReferenceMapping::SetIsReferenced(int idx, bool flag) {
  int bitIndex = idx & 0x40;
  int cellIndex = idx >> 6;
  
  if (!flag) {
    if (IsReferenced(idx)) {
      refs[cellIndex] ^= (1L << bitIndex);
    }
  } else {
    refs[cellIndex] |= (1L << bitIndex);
  }
}

ReferenceMapping::ReferenceMapping(PhysicalAllocator * allocator,
                                   StandaloneMapping * map)
                                   : StandaloneMapping(allocator) {
  for (int i = 0; i < 0x200; i++) {
    if (map->pml4[i] & 1) {
      SetIsReferenced(i, true);
      pml4[i] = map->pml4[i];
    } else {
      SetIsReferenced(i, false);
    }
  }
}

ReferenceMapping::~ReferenceMapping() {
  // unreference PDPTs that we don't own
  for (int i = 0; i < 0x200; i++) {
    if (IsReferenced(i)) {
      pml4[i] = 0;
    }
  }
}

void ReferenceMapping::Unmap(void * address) {
  uintptr_t pageIdx = (uintptr_t)address >> 12;
  uintptr_t pml4Off = (pageIdx >> 27);
  if (IsReferenced(pml4Off)) {
    Panic("ReferenceMapping::Unmap() - passed referenced address");
  }
  
  StandaloneMapping::Unmap(address);
}

bool ReferenceMapping::Map(void * address, void * phys,
                           uint64_t size, int flags) {
  uintptr_t pageIdx = (uintptr_t)address >> 12;
  uintptr_t pml4Off = (pageIdx >> 27);
  if (IsReferenced(pml4Off)) {
    Panic("ReferenceMapping::Map() - passed referenced address");
  }
  
  return StandaloneMapping::Map(address, phys, size, flags);
}

}
