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

#include "common-x64.h"

namespace OS {

namespace x64 {

void OutB(uint16_t port, uint8_t byte) {
  __asm__("outb %%al, %%dx" : : "a" (byte), "d" (port));
}

void CPUID(uint32_t eax, uint32_t * ebx, uint32_t * edx, uint32_t * ecx) {
  uint32_t ebxIn = ebx ? *ebx : 0;
  uint32_t ecxIn = ecx ? *ecx : 0;
  uint32_t edxIn = edx ? *edx : 0;
  uint64_t rbxOut, rcxOut, rdxOut;
  __asm__("cpuid"
          : "=b" (rbxOut), "=c" (rcxOut), "=d" (rdxOut)
          : "b" (ebxIn), "c" (ecxIn), "d" (edxIn));
  if (ebx) *ebx = (uint32_t)rbxOut;
  if (ecx) *ecx = (uint32_t)rcxOut;
  if (edx) *edx = (uint32_t)rdxOut;
}

}

}

