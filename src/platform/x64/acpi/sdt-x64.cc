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

#include "sdt-x64.h"

namespace OS {

namespace x64 {

namespace ACPI {

static bool hasFound = false;
static RSDP * rsdp = NULL;

static bool FindSDT();
static uint8_t MemChecksum(uint8_t * ptr, int len);

RSDP & GetRSDP() {
  if (!hasFound) {
    if (!FindSDT()) {
      Panic("OS::x64::ACPI::GetRSDP() - not found");
    }
    hasFound = true;
  }
  return *rsdp;
}

uint64_t RSDP::TableCount() {
  return 0; // TODO: nyi
}

void * RSDP::GetTable(uint64_t idx) {
  return NULL; // TODO: nyi
}

/***********
 * Private *
 ***********/

static bool FindSDT() {
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
