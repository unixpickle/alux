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

CPUID_1_FEATURES equ (1 << 3) | (1 << 6) | (1 << 24) | (1 << 26)
CPUID_80000001_FEATURES equ (1 << 11) | (1 << 29) | (1 << 20)

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

  ; ebx contains the multiboot info structure, so we push it along with
  ; a null dword so that the stack stays 64-bit aligned
  push ebx
  push dword 0

  ; NOTE: I may need to reset EFLAGS here, but I don't think so

  ; check CPUID(1)
  mov eax, 1
  cpuid
  mov eax, CPUID_1_FEATURES
  and eax, edx
  cmp eax, CPUID_1_FEATURES
  jne .errCPUID

  ; check CPUID(0x80000001)
  mov eax, 0x80000001
  cpuid
  mov eax, CPUID_80000001_FEATURES
  and eax, edx
  cmp eax, CPUID_80000001_FEATURES
  jne .errCPUID

  ; enable PAE and PGE
  mov eax, cr4
  or eax, 0xa0
  mov cr4, eax

  ; set Long Mode bit
  mov ecx, 0xc0000080
  rdmsr
  or eax, 0x101
  wrmsr

  ; configure paging
  mov esi, initial_pml4
  mov cr3, esi
  mov eax, cr0
  bts eax, 31
  mov cr0, eax

  ; switch to 64-bit code segment
  lgdt [gdt_pointer]
  jmp gdt.code:entry64

  ; print NYI error
  mov edi, .initialError
  call print_error
.initialError:
  db 'Not yet implemented :(', 0

.errCPUID:
  mov edi, .cpuidMsg
  call print_error
.cpuidMsg:
  db 'CPUID -- missing features!', 0

; pass the error string pointer in %edi
print_error:
  mov ah, 0x0a ; green
  mov edx, 0xb8000
.mainLoop:
  mov al, [edi]
  test al, al
  jz .donePrint
  
  mov [edx], ax
  add edx, 2
  
  inc edi
  jmp .mainLoop
.donePrint:
  hlt

bits 64

entry64:
  ; find signature '_X64_ENTRYPOINT!'
  mov rsi, LOADBASE + 0x5000
  mov rax, 0x544e455f3436585f ; first qword
  mov rbx, 0x21544e494f505952 ; second qword

.findLoop:
  cmp [rsi], rax
  je .testUpper ; only jump if we're basically sure we found it; branching hax
.continue:
  inc rsi
  jmp .findLoop
.testUpper:
  cmp [rsi + 8], rbx
  jne .continue

  ; entry point found!
  add rsi, 0x10
  jmp rsi

; static data area

align 8

initial_stack:
  times 0x1000 db 0x0
.end:

gdt:
    .null: equ $ - gdt
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
    .code: equ $ - gdt
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011000b                 ; Access.
    db 00100000b                 ; Granularity & 64-bit mode flag
    db 0                         ; Base (high).
    .data: equ $ - gdt
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access.
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .code_user: equ $ - gdt
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11111000b                 ; Access with DPL = 3
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
    .data_user: equ $ - gdt
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11110010b                 ; Access.
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
global gdt_pointer
gdt_pointer:
    dw $ - gdt - 1        ; Limit.
    dq gdt                ; Base.

align 0x1000
initial_pml4:
  dq initial_pdpt + 3
  times 0x1ff dq 0x0

align 0x1000
initial_pdpt:
  dq initial_pdt + 3
  times 0x1ff dq 0x0

initial_pdt:
  times 0x200 dq 0x83 + ((($ - initial_pdt) >> 3) << 0x200000)

; pad the result file to 0x100 bytes
times (0x5000 - ($ - multiboot_header)) db 0x0

