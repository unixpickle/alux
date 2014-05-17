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

#ifndef __PLATFORM_X64_MAP_SETUP_X64_H__
#define __PLATFORM_X64_MAP_SETUP_X64_H__

#include <cstring>
#include "page-allocator-x64.hpp"
#include "size-config-x64.hpp"

namespace OS {

namespace x64 {

/**
 * The lifecycle of this class assumes you are in the physical address space--
 * that is, an identity mapped virtual address space. In reality, only the
 * first GB is identity mapped, but that ought to be enough.
 */
class MapSetup {
private:
  PageAllocator * allocator;
  
  PhysAddr pdpt, pml4;
  int pdtOffset, pdptOffset;
  PhysAddr currentPDT;
  
  VirtAddr firstUnmappedVirtual;
  void MapNextVirtual();
  
public:
  MapSetup(PageAllocator * allocator);
  void Map();
  VirtAddr GetFirstUnmapped();
  PhysAddr GetPDPT();
  PhysAddr GetPML4();
  
};

}

}

#endif

