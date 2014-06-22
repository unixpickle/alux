section .text

global _Syscall
global Syscall

_Syscall:
Syscall:
  push rbx
  mov rbx, rcx
  syscall
  pop rbx
  ret