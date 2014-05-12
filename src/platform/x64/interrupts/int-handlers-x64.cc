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

#include "int-handlers-x64.h"

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code) {
  OS::cout << "OS::x64::InterruptCoded() - caller=" << (uintptr_t)caller
    << " vector=" << vector << " code=" << code << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptRegular(void * caller, uint64_t vector) {
  OS::cout << "OS::x64::InterruptRegular() - caller=" << (uintptr_t)caller
    << " vector=" << vector << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code) {
  InterruptCoded(caller, vector, code);
}

void _InterruptRegular(void * caller, uint64_t vector) {
  InterruptRegular(caller, vector);
}

}

namespace OS {

namespace x64 {

static InterruptTable globalIdt;
static void DummyHandler(void * ret, uint64_t vec);

void InitializeIDT() {
  new(&globalIdt) InterruptTable();
}

InterruptTable & GetGlobalIDT() {
  return globalIdt;
}

void ConfigureDummyIDT() {
  for (int i = 0; i < 0x100; i++) {
    globalIdt.SetHandler(i, (void *)DummyHandler, 0x8e);
  }
}

void ConfigureRealIDT() {
  // TODO: set RawCodedHandler and RawNonCodedHandler
}

static void DummyHandler(void * ret, uint64_t vec) {
  cout << "OS::x64::DummyHandler(" << (uintptr_t)ret << ", " << vec << ")"
    << endl;
}

}

}
