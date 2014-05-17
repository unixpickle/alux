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

void TableMgr::ClearMap(VirtAddr virt, size_t size) {
  while (size) {
    size_t gotSize = UnmapPage(virt, false);
    assert(gotSize <= size);
    virt += gotSize;
    size -= gotSize;
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
  assert(!(virt & 0x1fffff));
  assert(!(phys & 0x1fffff));
  assert(3 == (tableMask & 3));
  assert(0x80 & entryMask);
  
  int indexes[3] = {
    (int)((virt >> 39) & 0x1ff),
    (int)((virt >> 30) & 0x1ff),
    (int)((virt >> 21) & 0x1ff)
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

size_t TableMgr::UnmapPage(VirtAddr addr, bool setToZero) {
  assert(!(addr & 0xfff));
  int indexes[4] = {
    (int)((addr >> 39) & 0x1ff),
    (int)((addr >> 30) & 0x1ff),
    (int)((addr >> 21) & 0x1ff),
    (int)((addr >> 12) & 0x1ff)
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
  
  if (!setToZero) {
    scratch[indexes[maxDepth]] = 0x1000; // set it to some non-zero value
  } else {
    // zero out the page and delete any parent table indexes
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
      
      KernelMap::GetGlobal().allocator->FreePage(tableAddresses[i]);
      scratch.Reassign(tableAddresses[i - 1]);
      scratch[indexes[i - 1]] = 0;
    }
  }
  return 0x1000L << ((3 - maxDepth) * 9);
}

void TableMgr::ReadMap(VirtAddr start, size_t & sizeOut, bool & isMapped) {
  // get the status at `start`
  assert(!(start & 0xfff));
  int indexes[4] = {
    (int)((start >> 39) & 0x1ff),
    (int)((start >> 30) & 0x1ff),
    (int)((start >> 21) & 0x1ff),
    (int)((start >> 12) & 0x1ff)
  };
  
  TypedScratch<uint64_t> scratch(pml4);
  int depth = 0;
  for (depth = 0; depth < 3; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (nextPage & 0x80 || !(nextPage & 1)) {
      break;
    }
    scratch.Reassign(nextPage & 0x7ffffffffffff000);
  }
  
  if (scratch[indexes[depth]] & 0x80 || depth == 3) {
    isMapped = scratch[indexes[depth]] & 1;
  } else {
    isMapped = false;
  }
  
  sizeOut = 0x1000 << ((3 - depth) * 9);
  assert(!(start & (sizeOut - 1)));
}

void TableMgr::FindNewBU(VirtAddr & buStart, size_t & buSize,
                         VirtAddr maxAddr) {
  VirtAddr lastAddress = 0;
  buStart = 0;
  buSize = 0;
  while (lastAddress + buSize < maxAddr) {
    size_t chunkSize = 0;
    VirtAddr nextStart = lastAddress;
    while (1) {
      size_t size;
      bool mapped;
      ReadMap(lastAddress + chunkSize, size, mapped);
      if (mapped) {
        nextStart = lastAddress + chunkSize + size;
        break;
      }
      if (lastAddress + chunkSize + size > maxAddr) break;
      chunkSize += size;
    }
    
    if (chunkSize > buSize) {
      buStart = lastAddress;
      buSize = chunkSize;
    }
    lastAddress = nextStart;
  }
}

}

}
