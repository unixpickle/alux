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

#include "region.h"

namespace OS {

MemoryRegion::MemoryRegion(void * start, uintptr_t size) {
  this->start = start;
  this->size = size;
}

MemoryRegion::MemoryRegion(const MemoryRegion & region) {
  start = region.start;
  size = region.size;
}

MemoryRegion::MemoryRegion() {
  this->start = 0;
  this->size = 0;
}

void * MemoryRegion::GetStart() const {
  return start;
}

uintptr_t MemoryRegion::GetSize() const {
  return size;
}

void * MemoryRegion::GetEnd() const {
  return GetOffset(size);
}

void * MemoryRegion::GetOffset(uintptr_t aSize) const {
  return (void *)((uintptr_t)start + aSize);
}

MemoryRegion & MemoryRegion::operator=(const MemoryRegion & region) {
  start = region.start;
  size = region.size;
  return *this;
}

}
