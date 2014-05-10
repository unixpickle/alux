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

#include "table-mgr-x64.h"
#include "kernel-map-x64.h"

namespace OS {

namespace x64 {

TableMgr::TableMgr(KernelMap * map, PhysAddr _pml4) {
  kernMap = map;
  pml4 = _pml4;
}

TableMgr::TableMgr() {
  kernMap = NULL;
  pml4 = NULL;
}

TableMgr & TableMgr::operator=(const TableMgr & mgr) {
  kernMap = mgr.kernMap;
  pml4 = mgr.pml4;
  return *this;
}

void TableMgr::Set() {
  __asm__("mov %0, %%cr3" : : "r" (pml4));
}

void TableMgr::Map(VirtAddr virt, PhysAddr phys, size_t size, bool largePages,
                   uint64_t entryMask, uint64_t tablesMask) {
  Panic("TableMgr::Map() -- NYI");
}

void TableMgr::Unmap(VirtAddr virt, size_t size) {
  Panic("TableMgr::Unmap() -- NYI");
}

void TableMgr::FindNewBU(VirtAddr & buStart, size_t & buSize) {
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

bool TableMgr::FollowBigChunk(PhysAddr _table,
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
  } else if (!_table) {
    contigSize = (0x1000L << ((4 - depth) * 9)) - (regStart - mapAddr);
    return true;
  }
  
  // get the actual table address without any flags
  PhysAddr table = _table & 0x7ffffffffffff000L;
  
  size_t segmentSize = 0x1000L << ((3 - depth) * 9);
  int startIndex = (int)((regStart - mapAddr) / segmentSize);
  contigSize = 0;
  
  // loop through entries
  TypedScratch<uint64_t> scratch(kernMap, table);
  for (int i = startIndex; i < 0x200; i++) {
    uint64_t entry = scratch[i];
    size_t resSize;
    VirtAddr newMapAddr = mapAddr + (i * segmentSize);
    bool res = FollowBigChunk(entry, depth + 1, newMapAddr,
                              regStart < newMapAddr ? newMapAddr : regStart,
                              resSize);
    contigSize += resSize;
    if (!res) return false;
  }
  
  return true;
}

bool TableMgr::FindNextUnmapped(PhysAddr _table,
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
  } else if (!_table) {
    result = mapAddr;
    return true;
  }
  
  PhysAddr table = _table & 0x7ffffffffffff000L;
  
  size_t segmentSize = 0x1000L << ((3 - depth) * 9);
  int startIndex = (int)((start - mapAddr) / segmentSize);
  
  // loop through entries
  TypedScratch<uint64_t> scratch(kernMap, table);
  for (int i = startIndex; i < 0x200; i++) {
    uint64_t entry = scratch[i];
    VirtAddr newMapAddr = mapAddr + (i * segmentSize);
    bool res = FindNextUnmapped(entry, depth + 1, newMapAddr,
                                start < newMapAddr ? newMapAddr : start,
                                result);
    if (res) return true;
  }
  
  return false;
}

}

}
