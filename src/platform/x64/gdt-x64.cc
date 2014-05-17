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

/*

#include "gdt-x64.hpp"

namespace OS {

namespace x64 {

GDT::DescTSS::DescTSS(TSS * tss)
  : limit_0(0x67), type(9), res0(0), dpl(0), present(1), limit_16(0),
    available(1), res1(0), granularity(0), res2(0) {
  // as much as I love initializer lists, I like code too.
  uint64_t ptrVal = (uint64_t)tss;
  base_0 = ptrVal & 0xffff;
  base_16 = (ptrVal >> 16) & 0xff;
  base_24 = (ptrVal >> 24) & 0xff;
  base_32 = ptrVal >> 0x20;
}

GDTPointer & GDT::GetCurrentPtr() {
  // TODO: nyi
}

GDT & GDT::GetGlobal() {
  // TODO: nyi
}

GDT::GDT(const GDTPointer & currentPtr) {
  // TODO: nyi
}

uint16_t GDT::AddTSS(TSS * aTSS) {
  // TODO: nyi
}

};

}

}

*/
