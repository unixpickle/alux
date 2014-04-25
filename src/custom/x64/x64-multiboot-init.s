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

%include "./addresses.s"
org LOADBASE

bits 32

; these are multiboot flags
MBALIGN equ 1<<0
MEMINFO equ 1<<1
VIDEOINFO equ 1<<2
LINKINFO equ 1<<16
FLAGS equ LINKINFO | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

multiboot_header:
  dd MAGIC
  dd FLAGS
  dd CHECKSUM
  dd multiboot_header ; header pointer
  dd LOADBASE         ; base pointer
  dd 0                ; loadEndAddr - can be 0 to default
  dd 0                ; bssEndAddr, 0 = no BSS segment
  dd start            ; entry point

start:
  mov esp, initial_stack.end
  mov ebp, esp
  
  ; generate page tables
  mov edi, .initialError
  call print_error
.initialError:
  db 'Not yet implemented :(', 0

; move error string to edi
print_error:
  mov ah, 0x0a ; green
  mov edx, 0xb8000
.mainLoop:
  mov al, [edi]
  test al, al
  jz .donePrint
  
  mov [edx], ax
  add ecx, 2
  
  inc edi
  jmp .mainLoop
.donePrint:
  hlt

; static data area

initial_stack:
  times 0x1000 db 0x0
.end:

align 0x1000
initial_pml4:
  dq initial_pdpt + 3
  times 0x1ff dq 0x0

align 0x1000
initial_pdpt:
  dq 0b110000011 ; global, granularity (1GB), writable, user
  times 0x1ff dq 0x0

; pad the result file to 0x100 bytes
times (0x4000 - ($ - multiboot_header)) db 0x0

