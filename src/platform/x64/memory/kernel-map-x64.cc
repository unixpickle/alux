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

KernelMap::KernelMap() {
  scratchLock = 0;
  bzero(scratchBitmaps, sizeof(scratchBitmaps));
}

PhysAddr KernelMap::Setup(PhysRegionList * regs) {
  MapSetup setup(regs);
  setup.Map();
  
  assert(ScratchPTCount <= 0x200);
  assert(setup.GetFirstUnmapped() <= ScratchStartAddr);
  
  PhysAddr scratchPDT = setup.AllocPage();
  ((uint64_t *)setup.GetPDPT())[0x1ff] = scratchPDT | 3;
  uint64_t scratchStart = (uint64_t)ScratchPTStart();
  for (int i = 0; i < ScratchPTCount; i++) {
    PhysAddr scratchPT = scratchStart + (i << 12);
    ((uint64_t *)scratchPDT)[i] = scratchPT | 3;
  }
  
  pml4 = setup.GetPML4();
  pdpt = setup.GetPDPT();
  
  return setup.GetFirstFree();
}

void KernelMap::Set() {
  __asm__("mov %0, %%cr3" : : "r" (pml4));
}

PhysAddr KernelMap::GetPDPT() {
  return pdpt;
}

VirtAddr KernelMap::Map(PhysAddr start, size_t size, bool largePages) {
  assert(!(size & (largePages ? 0x1fffff : 0xfff)));
  assert(!(start & (largePages ? 0x1fffff : 0xfff)));
  
  ScopeLock scope(&mapLock);
  
  // see if we can find a place in our biggest unused
  if (!CanFitRegion(size, largePages)) {
    FindNewBU();
    if (!CanFitRegion(size, largePages)) {
      return 0;
    }
  }
  
  // align our BU
  if (largePages) {
    buSize -= 0x20000 - (buStart & 0x1fffff);
    buStart += 0x20000 - (buStart & 0x1fffff);
    
    // map starting at buStart
    MapAt(buStart, start, size, largePages);
    buStart += size;
    buSize -= size;
  }
  
  return 0;
}

void KernelMap::MapAt(VirtAddr virt, PhysAddr start,
                      size_t size, bool largePages) {
  assert(!(size & (largePages ? 0x1fffff : 0xfff)));
  assert(!(start & (largePages ? 0x1fffff : 0xfff)));
  assert(!(virt & (largePages ? 0x1fffff : 0xfff)));
  
  // here, manually manipulate the page tables
  Panic("KernelMap::MapAt() - NYI");
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

void KernelMap::FreeScratch(VirtAddr ptr) {
  ScopeLock scope(&scratchLock);
  
  int bitIndex = (int)((ptr - ScratchStartAddr) >> 12);
  int fieldIndex = bitIndex / 0x40;
  scratchBitmaps[fieldIndex] ^= 1L << (bitIndex & 0x3f);
}

/***********
 * PRIVATE *
 ***********/

void KernelMap::FindNewBU() {
  VirtAddr lastAddress = 0;
  buStart = 0;
  buSize = 0;
  while (lastAddress + buSize < 0x8000000000L) {
    bool res = FindNextUnmapped(pml4, 0, 0, lastAddress, lastAddress);
    if (!res) break;
    size_t retSize = 0;
    FollowBigChunk(pml4, 0, 0, lastAddress, retSize);
    if (retSize > buSize) {
      buStart = lastAddress;
      buSize = retSize;
    }
    lastAddress += retSize;
  }
}

bool KernelMap::FollowBigChunk(PhysAddr _table,
                               int depth,
                               VirtAddr mapAddr,
                               VirtAddr regStart,
                               size_t & contigSize) {
  assert(!(regStart & 0xfff));
  assert(!(mapAddr & 0xfff));
  // check for data entries
  if (depth == 4) {
    if (_table) {
      contigSize = 0;
      return false;
    } else {
      contigSize = 0x1000;
      return true;
    }
  } else if (_table & 0x80) {
    contigSize = 0;
    return false;
  }
  
  PhysAddr table = _table & 0x7ffffffffffff000L;
  
  size_t segmentSize = 0x1000L << ((3 - depth) * 9);
  int startIndex = (int)((regStart - mapAddr) / segmentSize);
  contigSize = 0;
  
  uint64_t * scratch = (uint64_t *)AllocScratch(table);
  if (!scratch) Panic("KernelMap::FollowBigChunk() - no scratch");
  
  // loop through entries
  for (int i = startIndex; i < 0x200; i++) {
    uint64_t entry = scratch[i];
    size_t resSize;
    VirtAddr newMapAddr = mapAddr + (i * segmentSize);
    bool res = FollowBigChunk(entry, depth + 1, newMapAddr,
                              regStart < newMapAddr ? newMapAddr : regStart,
                              resSize);
    contigSize += resSize;
    if (!res) {
      FreeScratch(table);
      return false;
    }
  }
  
  FreeScratch((VirtAddr)scratch);
  return true;
}

bool KernelMap::FindNextUnmapped(PhysAddr _table,
                                 int depth,
                                 VirtAddr mapAddr,
                                 VirtAddr start,
                                 VirtAddr & result) {
  assert(!(start & 0xfff));
  assert(!(mapAddr & 0xfff));
  
  // check for data entries
  if (depth == 4) {
    if (!_table) {
      result = mapAddr;
      return true;
    } else {
      return false;
    }
  } else if (_table & 0x80) {
    return false;
  }
  
  PhysAddr table = _table & 0x7ffffffffffff000L;
  
  size_t segmentSize = 0x1000L << ((3 - depth) * 9);
  int startIndex = (int)((start - mapAddr) / segmentSize);
  
  uint64_t * scratch = (uint64_t *)AllocScratch(table);
  if (!scratch) Panic("KernelMap::FollowBigChunk() - no scratch");
  
  // loop through entries
  for (int i = startIndex; i < 0x200; i++) {
    uint64_t entry = scratch[i];
    VirtAddr newMapAddr = mapAddr + (i * segmentSize);
    bool res = FindNextUnmapped(entry, depth + 1, newMapAddr,
                                start < newMapAddr ? newMapAddr : start,
                                result);
    if (res) {
      FreeScratch(table);
      return true;
    }
  }
  
  FreeScratch((VirtAddr)scratch);
  return false;
}

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