section .text

global RawNonCodedHandler
global RawCodedHandler
global RawDummyHandler
global _RawNonCodedHandler
global _RawCodedHandler
global _RawDummyHandler

extern _InterruptCoded, _InterruptRegular, _InterruptDummy

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

RawDummyHandler:
  pushunpres
  
  mov rsi, [rsp + 0x48] ; vector number
  mov rdi, [rsp + 0x50] ; caller RIP
  call _InterruptDummy
  
  popunpres
  add rsp, 0x8
  iretq

_RawNonCodedHandler:
  jmp RawNonCodedHandler

_RawCodedHandler:
  jmp RawCodedHandler

_RawDummyHandler:
  jmp RawDummyHandler

