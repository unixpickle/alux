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

#include "root-map-x64.h"

namespace OS {

static RootMapper * mapper = NULL;

void MultibootRootMapper::SetRootMapper(RootMapper * x) {
  mapper = x;
}

RootMapper * RootMapper::GetRootMapper() {
  return mapper;
}

MultibootRootMapper::MultibootRootMapper(void * multibootPtr) {
  // TODO: here, calculate the physical regions using the multiboot
  // information.
}

MemoryRegion * MultibootRootMapper::PhysicalRegions() {
  return regions;
}

int MultibootRootMapper::PhysicalRegionCount() {
  return regionCount;
}

void * MultibootRootMapper::FirstFreeVirtual() {
  // TODO: here, use some sort of symbol to calculate this
  return (void *)0x1000000; // 16MB mark
}

void * MultibootRootMapper::FirstFreePhysical() {
  return FirstFreeVirtual();
}

}
