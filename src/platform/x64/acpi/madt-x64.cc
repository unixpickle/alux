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

#include "madt-x64.h"

namespace OS {

namespace x64 {

namespace ACPI {

static MADT globalMadt;
static bool hasInitialized = false;
static bool hasFound = false;

MADT::MADT(PhysAddr physPtr) : tableCount(0) {
  MemcpyToVirt((VirtAddr)&header, physPtr, HeaderSize);
  dataSize = header.length - HeaderSize;
  
  if (dataSize) {
    data = new uint8_t[dataSize];
    MemcpyToVirt((VirtAddr)data, physPtr + HeaderSize, dataSize);
    
    // count the tables
    size_t offset = 0;
    while (offset + 2 < dataSize) {
      uint8_t len = data[1 + offset];
      offset += len;
      tableCount++;
    }
  } else {
    data = NULL;
  }
}

MADT::~MADT() {
  if (data) delete data;
}

const MADTHeader & MADT::GetHeader() {
  return header;
}

const void * MADT::GetData() {
  return data;
}

size_t MADT::GetDataSize() {
  return dataSize;
}

int MADT::GetTableCount() {
  return tableCount;
}

void * MADT::GetTable(int i) {
  assert(i >= 0 && i < tableCount);
  
  int index = 0;
  size_t offset = 0;
  for (int j = 0; j < i; j++) {
    offset += data[1 + offset];
  }
  return (void *)(data + offset);
}

MADT * GetMADT() {
  // the MADT will be initialized before we have to worry about SMP and locking
  if (hasInitialized) {
    if (!hasFound) return NULL;
    return &globalMadt;
  }
  
  // find the MADT
  RSDP rsdp = GetRSDP();
  for (uint64_t i = 0; i < rsdp.TableCount(); i++) {
    PhysAddr table = rsdp.GetTable(i);
    uint32_t sig = 0;
    MemcpyToVirt((VirtAddr)&sig, table, 4);
    if (sig == 0x43495041) {
      new(&globalMadt) MADT(table);
      hasFound = true;
      break;
    }
  }
  
  hasInitialized = true;
  return GetMADT();
}

}

}

}
