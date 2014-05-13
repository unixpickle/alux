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

#include "rsdp-x64.h"

namespace OS {

namespace x64 {

namespace ACPI {

static RSDP * rsdp = NULL;

static bool FindRSDP();
static uint8_t MemChecksum(uint8_t * ptr, int len);

RSDP & GetRSDP() {
  if (!rsdp) {
    if (!FindRSDP()) {
      Panic("OS::x64::ACPI::GetRSDP() - not found");
    }
  }
  return *rsdp;
}

uint64_t RSDP::TableCount() {
  if (revision == 0) {
    // use RSDT
    uint32_t lenField = 0;
    uint64_t offset = (uint64_t)ptrRSDT;
    MemcpyToVirt((VirtAddr)&lenField, (PhysAddr)(offset + 4), 4);
    return (lenField - 0x24) >> 2;
  } else {
    // use XSDT    
    uint32_t lenField = 0;
    uint64_t offset = ptrXSDT;
    MemcpyToVirt((VirtAddr)&lenField, (PhysAddr)(offset + 4), 4);
    return (lenField - 0x24) >> 3;
  }
}

PhysAddr RSDP::GetTable(uint64_t idx) {
  if (rsdp->revision == 0) {
    // use RSDT
    uint64_t off = 0x24 + (idx << 2);
    uint64_t res = 0; // must initialize to 0!
    uint64_t offset = (uint64_t)ptrRSDT + off;
    MemcpyToVirt((VirtAddr)&res, (PhysAddr)offset, 4);
    return res;
  } else {
    // use XSDT
    uint64_t off = 0x24 + (idx << 3);
    uint64_t res;
    MemcpyToVirt((VirtAddr)&res, (PhysAddr)(ptrXSDT + off), 8);
    return res;
  }
}

/***********
 * Private *
 ***********/

static bool FindRSDP() {
  // find the RSDP in the BIOS areas.
  const char * signature = "RSD PTR ";
  uintptr_t ptr;
  
  // the whole potential EBDA
  for (ptr = 0x80000; ptr < 0x9fc00; ptr++) {
    if (memcmp(signature, (void *)ptr, 8) == 0) {
      if (MemChecksum((uint8_t *)ptr, 0x14) != 0) continue;
      rsdp = (RSDP *)ptr;
      return true;
    }
  }
  for (ptr = 0xe0000; ptr < 0x100000; ptr++) {
    if (memcmp(signature, (void *)ptr, 8) == 0) {
      if (MemChecksum((uint8_t *)ptr, 0x14) != 0) continue;
      rsdp = (RSDP *)ptr;
      return true;
    }
  }
  
  return false;
}

static uint8_t MemChecksum(uint8_t * ptr, int len) {
  uint8_t sum = 0;
  while (len-- > 0) {
    sum += (*ptr);
    ptr++;
  }
  return sum;
}

}

}

}
