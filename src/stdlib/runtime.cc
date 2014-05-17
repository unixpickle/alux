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

#include "runtime.hpp"
#include <platform/failure.hpp>
#include <memory/kmalloc.hpp>

void * operator new(size_t, void * p) {
  return p;
}

void * operator new[](size_t, void * p) {
  return p;
}

void * operator new(size_t s) {
  return OS::Malloc(s);
}

void * operator new[](size_t s) {
  return OS::Malloc(s);
}

void operator delete(void * p) {
  OS::Free(p);
}

void operator delete[](void * p) {
  OS::Free(p);
}

void operator delete(void *, void *) {
}

void operator delete[](void *, void *) {
}

extern "C" {

void * __dso_handle = 0;

int __cxa_atexit(void (* destructor) (void *), void * arg, void * dso) {
  (void)destructor;
  (void)arg;
  (void)dso;
  return 0;
}

void __cxa_finalize(void * f) {
  (void)f;
}

void __cxa_pure_virtual() {
  OS::Panic("__cxa_pure_virtual() has been called");
}

}
