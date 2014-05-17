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

#ifndef __PLATFORM_X64_GDT_X64_H__
#define __PLATFORM_X64_GDT_X64_H__

#include <new>
#include "common-x64.hpp"

namespace OS {

namespace x64 {

class TSS {
  uint32_t res0;
  uint64_t rsp[3];
  uint64_t res1;
  uint64_t ist[7];
  uint64_t res2;
  uint16_t res3;
  uint16_t ioplBase;
} OS_PACKED;

class GDT {
public:
  class DescTSS {
    uint16_t limit_0;
    uint16_t base_0;
    uint8_t base_16;
    unsigned type : 4;
    unsigned res0 : 1;
    unsigned dpl : 2;
    unsigned present : 1;
    unsigned limit_16 : 4;
    unsigned available : 1;
    unsigned res1 : 2;
    unsigned granularity : 1;
    uint8_t base_24;
    uint32_t base_32;
    uint32_t res2;

    DescTSS(TSS * tss);
  } OS_PACKED;

  class GDTPointer {
    uint16_t limit;
    uint64_t start;
  } OS_PACKED;

private:
  void * buffer;
  size_t offset;

public:
  static GDTPointer & GetCurrentPtr(); // uses SGDT
  static GDT & GetGlobal();

  GDT() {
    Panic("This initializer is for the compiler only");
  }

  GDT(const GDTPointer & currentPtr);
  uint16_t AddTSS(TSS * aTSS);

};

}

}

#endif
