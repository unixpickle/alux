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

#ifndef __PLATFORM_X64_VM_X64_H__
#define __PLATFORM_X64_VM_X64_H__

#include <platform/vm.h>
#include <utilities/lock.h>
#include <new>
#include <platform/failure.h>
#include "common-x64.h"

namespace OS {

class StandaloneMapping : public VirtualMapping {
protected:
  uintptr_t * pml4;
  PhysicalAllocator * allocator;

  void FreeTable(uintptr_t * table, int depth);
  friend class ReferenceMapping;
  
public:
  StandaloneMapping(PhysicalAllocator * allocator);
  virtual ~StandaloneMapping();
  virtual void SetAllocator(PhysicalAllocator * alloc);
  virtual void Unmap(void * address);
  virtual bool Map(void * address, void * phys, uint64_t size, int flags);
  virtual bool Lookup(void * address,
              void ** phys,
              void ** start,
              int * flags,
              uint64_t * size);
  virtual void MakeCurrent();
};

class ReferenceMapping : public StandaloneMapping {
private:
  uint64_t refs[8]; // bitmap
  bool IsReferenced(int idx);
  void SetIsReferenced(int idx, bool flag);

public:
  ReferenceMapping(PhysicalAllocator * allocator, StandaloneMapping * map);
  virtual ~ReferenceMapping();
  
  virtual void Unmap(void * address);
  virtual bool Map(void * address, void * phys, uint64_t size, int flags);
};

}

#endif
