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

#include "kernel-map-x64.h"

namespace OS {

namespace x64 {

static const VirtAddr ScratchStartAddr = 0x7FC0000000L;

KernelMap::KernelMap() : manager() {
  scratchLock = 0;
  bzero(scratchBitmaps, sizeof(scratchBitmaps));
}

void KernelMap::Setup() {
  MapSetup setup(allocator);
  setup.Map();
  
  assert(ScratchPTCount <= 0x200);
  assert(setup.GetFirstUnmapped() <= ScratchStartAddr);
  
  PhysAddr scratchPDT = allocator->AllocPage();
  bzero((void *)scratchPDT, 0x1000);
  
  ((uint64_t *)setup.GetPDPT())[0x1ff] = scratchPDT | 3;
  uint64_t scratchStart = (uint64_t)ScratchPTStart();
  for (int i = 0; i < ScratchPTCount; i++) {
    PhysAddr scratchPT = scratchStart + (i << 12);
    ((uint64_t *)scratchPDT)[i] = scratchPT | 3;
  }
  
  TableMgr man(this, setup.GetPML4());
  manager = man;
}

void KernelMap::Set() {
  manager.Set();
}

VirtAddr KernelMap::Map(PhysAddr start, size_t size, bool largePages) {
  assert(!(size & (largePages ? 0x1fffff : 0xfff)));
  assert(!(start & (largePages ? 0x1fffff : 0xfff)));
  
  ScopeLock scope(&mapLock);
  
  // see if we can find a place in our biggest unused
  if (!CanFitRegion(size, largePages)) {
    manager.FindNewBU(buStart, buSize, ScratchStartAddr);
    if (!CanFitRegion(size, largePages)) {
      return 0;
    }
  }
  
  // align our BU
  if (largePages) {
    buSize -= 0x20000 - (buStart & 0x1fffff);
    buStart += 0x20000 - (buStart & 0x1fffff);
  }
  
  // map starting at buStart
  uint64_t flags = 0x103 | (largePages ? 0x80 : 0);
  manager.Map(buStart, start, size, largePages, flags, 3);
  VirtAddr result = buStart;
  buStart += size;
  buSize -= size;
  
  return result;
}

void KernelMap::MapAt(VirtAddr virt, PhysAddr start,
                      size_t size, bool largePages) {
  ScopeLock scope(&mapLock);
  uint64_t flags = 0x103 | (largePages ? 0x80 : 0);
  manager.Map(virt, start, size, largePages, flags, 3);
  
  // if we are in the BU, we need to modify the BU.
  if (start >= buStart && start < buStart + buSize) {
    if (start + size >= buStart + buSize) {
      buStart = 0;
      buSize = 0;
    } else {
      buSize = buStart + buSize - (start + size);
      buStart = start + size;
    }
  }
}

VirtAddr KernelMap::AllocScratch(PhysAddr start) {
  ScopeLock scope(&scratchLock);
  
  assert(!(start & 0xfff));
  
  int bitIndex = -1;
  for (int i = 0; i < ScratchPTCount * 8; i++) {
    if (!~scratchBitmaps[i]) continue;
    // find the first free NULL
    for (int j = 0; j < 0x40; j++) {
      if (scratchBitmaps[i] & (1L << j)) continue;
      
      bitIndex = j + (i * 0x40);
      scratchBitmaps[i] |= (1L << j);
      break;
    }
    break;
  }
  if (bitIndex < 0) return 0;
  
  VirtAddr virt = ScratchStartAddr + (bitIndex << 12);
  int scratchTableIdx = bitIndex >> 9;
  uint64_t * table = (uint64_t *)((uint64_t)ScratchPTStart()
    + (scratchTableIdx << 12));
  table[bitIndex & 0x1ff] = start | 3;
  __asm__("invlpg (%0)" : : "r" (virt));
  
  return virt;
}

void KernelMap::ReassignScratch(VirtAddr addr, PhysAddr newAddr) {
  int bitIndex = (int)((addr - ScratchStartAddr) >> 12);
  int scratchTableIdx = bitIndex >> 9;
  uint64_t * table = (uint64_t *)((uint64_t)ScratchPTStart()
    + (scratchTableIdx << 12));
  table[bitIndex & 0x1ff] = newAddr | 3;
  __asm__("invlpg (%0)" : : "r" (addr));
}

void KernelMap::FreeScratch(VirtAddr ptr) {
  ScopeLock scope(&scratchLock);
  
  int bitIndex = (int)((ptr - ScratchStartAddr) >> 12);
  int fieldIndex = bitIndex / 0x40;
  scratchBitmaps[fieldIndex] ^= 1L << (bitIndex & 0x3f);
}

/***********
 * PRIVATE *
 ***********/

bool KernelMap::CanFitRegion(size_t size, bool bigPages) {
  if (!bigPages) {
    return size <= buSize;
  }
  if (buSize < 0x200000) return false;
  size_t realSize = buSize - (0x200000 - (buStart & 0x1fffff));
  return size <= realSize;
}

}

}