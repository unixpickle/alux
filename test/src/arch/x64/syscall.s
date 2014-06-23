section .text

align 8
runningCount:
  dq 1

global _Syscall
global Syscall

_Syscall:
Syscall:
  push rbx
  mov rbx, rcx
  syscall
  pop rbx
  ret

global _ThreadEntry
global ThreadEntry

_ThreadEntry:
ThreadEntry:
  mov rax, runningCount ; 64-bit relocation
  lock inc qword [rax]
  mov [rdi], rsp
  mov rbp, rsp
  mov rax, [rdi + 8]
  mov rdi, [rdi + 0x10]
  call rax

global _ThreadExit
global ThreadExit

_ThreadExit:
ThreadExit:
  mov rax, runningCount ; 64-bit relocation
  lock dec qword [rax]
  jz .killTask
.killThread:
  mov rdi, 4
  syscall
.killTask:
  mov rdi, 3
  xor r8, r8 ; status code
  syscall
