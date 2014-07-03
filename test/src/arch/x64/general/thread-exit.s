section .text

align 8
runningCount:
  dq 1

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
