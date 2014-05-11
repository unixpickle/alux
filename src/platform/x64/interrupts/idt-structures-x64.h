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

#ifndef __PLATFORM_X64_IDT_STRUCTURES_X64_H__
#define __PLATFORM_X64_IDT_STRUCTURES_X64_H__

#include <utilities/common.h>
#include <cstdint>
#include <cstring>

namespace OS {

namespace x64 {

// NOTICE: the C++11 standard is needed for this file because it uses packed
// structures with initializers.

/**
 * Represents this x86-64 machine code:
 * 
 * pushq %rax
 * pushq %rax
 * movabsq $argument, %rax
 * movq %rax, 0x8(%rsp)
 * movabsq $function, %rax
 * xchgq %rax, (%rsp)
 * ret
 */
class IntHandler {
public:
  char code1[4];
  uint64_t argument;
  char code2[7];
  uint64_t function;
  char code3[5];
  
  IntHandler() {
    memcpy(code1, "\x50\x50\x48\xB8", 4);
    memcpy(code2, "\x48\x89\x44\x24\x08\x48\xB8", 7);
    memcpy(code1, "\x48\x87\x04\x24\xC3", 5);
    argument = function = 0;
  }
} OS_PACKED; // size should == 0x20

class IdtEntry {
public:
  uint16_t lowOffset;
  uint16_t codeSegment;
  uint8_t reserved1;
  uint8_t flags;
  uint16_t midOffset;
  uint32_t highOffset;
  uint32_t reserved2;
  
  IdtEntry() {
    codeSegment = 8;
    reserved1 = 0;
    reserved2 = 0;
  }
  
  void SetOffset(uint64_t exc) {
    lowOffset = exc & 0xffff;
    midOffset = (exc >> 16) & 0xffff;
    highOffset = exc >> 32;
  }
} OS_PACKED; // size should == 0x10

class IdtPointer {
  
} OS_PACKED; // size should == 0xa0

}

}

#endif

