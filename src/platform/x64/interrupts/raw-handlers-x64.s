; Copyright (c) 2014, Alex Nichol and Alux contributors.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
; 
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.

section .text

global RawNonCodedHandler
global RawCodedHandler
global _RawNonCodedHandler
global _RawCodedHandler

extern _InterruptCoded, _InterruptRegular

%macro pushunpres 0
  push rdi
  push rsi
  push rdx
  push rcx
  push r8
  push r9
  push r10
  push r11
  push rax
%endmacro

%macro popunpres 0
  pop rax
  pop r11
  pop r10
  pop r9
  pop r8
  pop rcx
  pop rdx
  pop rsi
  pop rdi
%endmacro

RawNonCodedHandler:
  pushunpres
  
  mov rsi, [rsp + 0x48] ; vector number
  mov rdi, [rsp + 0x50] ; caller RIP
  call _InterruptRegular
  
  popunpres
  add rsp, 0x8
  iretq

RawCodedHandler:
  pushunpres
  
  mov rdx, [rsp + 0x50] ; exception code
  mov rsi, [rsp + 0x48] ; vector number
  mov rdi, [rsp + 0x58] ; caller RIP
  call _InterruptCoded
  
  popunpres
  add rsp, 0x10
  iretq

_RawNonCodedHandler:
  jmp RawNonCodedHandler

_RawCodedHandler:
  jmp RawCodedHandler
