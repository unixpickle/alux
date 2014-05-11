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

#include "entry.h"

namespace OS {

void EntryPoint() {
  size_t amountFree = PhysicalAvailable();
  size_t amountTaken = PhysicalUsed();
  cout << "OS::EntryPoint() - using " << amountTaken
    << " bytes out of " << amountTaken + amountFree << endl;
  
  InitializeMalloc();
  
  // test our memory allocator
  cout << "attempting to allocate 50 bytes" << endl;
  uint8_t * buffer = new uint8_t[50];
  cout << "returned pointer is " << (uintptr_t)buffer << endl;
  delete buffer;
  buffer = new uint8_t[50];
  cout << "new buffer is " << (uintptr_t)buffer << endl;
  delete buffer;
  
  amountFree = PhysicalAvailable();
  amountTaken = PhysicalUsed();
  cout << "OS::EntryPoint() - using " << amountTaken
    << " bytes out of " << amountTaken + amountFree << endl;
  
  cout << "allocating 0x100000 bytes, two times" << endl;
  uint8_t * b1 = new uint8_t[0x100000];
  uint8_t * b2 = new uint8_t[0x100000];
  
  cout << "b1=" << (uintptr_t)b1 << " b2=" << (uintptr_t)b2 << endl;
  delete b1;
  delete b2;
  
  amountFree = PhysicalAvailable();
  amountTaken = PhysicalUsed();
  cout << "OS::EntryPoint() - using " << amountTaken
    << " bytes out of " << amountTaken + amountFree << endl;
  
  Panic("nothing to do now");
}

}
