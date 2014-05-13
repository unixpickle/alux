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

TableMgr::TableMgr(PhysAddr _pml4) {
  pml4 = _pml4;
}

TableMgr::TableMgr() {
  pml4 = NULL;
}

TableMgr & TableMgr::operator=(const TableMgr & mgr) {
  pml4 = mgr.pml4;
  return *this;
}

void TableMgr::Set() {
  __asm__("mov %0, %%cr3" : : "r" (pml4));
}

void TableMgr::Map(VirtAddr virt, PhysAddr phys, size_t size, bool largePages,
                   uint64_t entryMask, uint64_t tableMask) {
  assert(size > 0);
  assert(3 == (tableMask & 3));
  
  // loop this way
  if (!largePages) {
    assert(!(size & 0xfff));
    for (size_t offset = 0; offset < size; offset += 0x1000) {
      MapSmallPage(virt + offset, phys + offset, entryMask, tableMask);
    }
  } else {
    assert(!(size & 0x1fffff));
    for (size_t offset = 0; offset < size; offset += 0x200000) {
      MapLargePage(virt + offset, phys + offset, entryMask, tableMask);
    }
  }
}

void TableMgr::Unmap(VirtAddr virt, size_t size) {
  while (size) {
    size_t gotSize = UnmapPage(virt);
    assert(gotSize <= size);
    virt += gotSize;
    size -= gotSize;
  }
}

void TableMgr::MapSmallPage(VirtAddr virt,
                            PhysAddr phys,
                            uint64_t entryMask,
                            uint64_t tableMask) {
  VirtAddr nonCanon = virt & 0x0000FFFFFFFFFFFFL;
  assert(!(virt & 0xfff));
  assert(!(phys & 0xfff));
  assert(3 == (tableMask & 3));
  int indexes[4] = {
    (int)((nonCanon >> 39) & 0x1ff),
    (int)((nonCanon >> 30) & 0x1ff),
    (int)((nonCanon >> 21) & 0x1ff),
    (int)((nonCanon >> 12) & 0x1ff)
  };
  
  TypedScratch<uint64_t> scratch(pml4);
  for (int depth = 0; depth < 3; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (!(nextPage & 1)) {
      nextPage = KernelMap::GetGlobal().allocator->AllocPage();
      
      TypedScratch<uint64_t> temp(nextPage);
      for (int i = 0; i < 0x200; i++) {
        temp[i] = 0;
      }
      
      scratch[indexes[depth]] = nextPage | tableMask;
    } else if (nextPage & 0x80) {
      Panic("TableMgr::MapSmallPage() - large page in the way!");
    } else {
      scratch[indexes[depth]] |= tableMask;
    }
    scratch.Reassign(nextPage & 0x7ffffffffffff000);
  }
  
  assert(!scratch[indexes[3]]);
  scratch[indexes[3]] = phys | entryMask;
}

void TableMgr::MapLargePage(VirtAddr virt,
                            PhysAddr phys,
                            uint64_t entryMask,
                            uint64_t tableMask) {
  VirtAddr nonCanon = virt & 0x0000FFFFFFFFFFFFL;
  assert(!(virt & 0x1fffff));
  assert(!(phys & 0x1fffff));
  assert(3 == (tableMask & 3));
  assert(0x80 & entryMask);
  
  int indexes[3] = {
    (int)((nonCanon >> 39) & 0x1ff),
    (int)((nonCanon >> 30) & 0x1ff),
    (int)((nonCanon >> 21) & 0x1ff)
  };
  
  TypedScratch<uint64_t> scratch(pml4);
  for (int depth = 0; depth < 2; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (!(nextPage & 1)) {
      nextPage = KernelMap::GetGlobal().allocator->AllocPage();
      
      TypedScratch<uint64_t> temp(nextPage);
      for (int i = 0; i < 0x200; i++) {
        temp[i] = 0;
      }
      
      scratch[indexes[depth]] = nextPage | tableMask;
    } else if (nextPage & 0x80) {
      Panic("TableMgr::MapLargePage() - large page in the way!");
    } else {
      scratch[indexes[depth]] |= tableMask;
    }
    scratch.Reassign(nextPage & 0x7ffffffffffff000);
  }
  
  assert(!scratch[indexes[2]]);
  scratch[indexes[2]] = phys | entryMask;
}

size_t TableMgr::UnmapPage(VirtAddr addr) {
  VirtAddr nonCanon = addr & 0x0000FFFFFFFFFFFFL;
  assert(!(addr & 0xfff));
  int indexes[4] = {
    (int)((nonCanon >> 39) & 0x1ff),
    (int)((nonCanon >> 30) & 0x1ff),
    (int)((nonCanon >> 21) & 0x1ff),
    (int)((nonCanon >> 12) & 0x1ff)
  };
  
  PhysAddr tableAddresses[4] = {pml4, 0, 0, 0};
  
  TypedScratch<uint64_t> scratch(pml4);
  int maxDepth = 3;
  for (int depth = 0; depth < 3; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (nextPage & 0x80) {
      maxDepth = depth;
      break;
    } else if (!(nextPage & 1)) {
      Panic("TableMgr::UnmapPage() - page not mapped!");
    }
    tableAddresses[depth + 1] = nextPage & 0x7ffffffffffff000;
    scratch.Reassign(tableAddresses[depth + 1]);
  }
  
  assert(scratch[indexes[maxDepth]]);
  scratch[indexes[maxDepth]] = 0;
  for (int i = maxDepth; i > 0; i--) {
    bool allGone = true;
    for (int j = 0; j < 0x200; j++) {
      if (scratch[j]) {
        allGone = false;
        break;
      }
    }
    if (!allGone) break;
    
    scratch.Reassign(tableAddresses[i - 1]);
    scratch[indexes[i - 1]] = 0;
  }
  return 0x1000L << ((3 - maxDepth) * 9);
}

void TableMgr::FindNewBU(VirtAddr & buStart, size_t & buSize,
                         VirtAddr maxAddr) {
  VirtAddr lastAddress = 0;
  buStart = 0;
  buSize = 0;
  while (lastAddress + buSize < maxAddr) {
    bool res = FindNextUnmapped(pml4, 0, 0, lastAddress, lastAddress);
    if (!res) break;
    if (lastAddress >= maxAddr) break;
    
    size_t retSize = 0;
    FollowBigChunk(pml4, 0, 0, lastAddress, retSize);
    
    if (retSize + lastAddress > maxAddr) {
      retSize = maxAddr - lastAddress;
    }
    
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
  TypedScratch<uint64_t> scratch(table);
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
  TypedScratch<uint64_t> scratch(table);
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
