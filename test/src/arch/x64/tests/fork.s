section .text

global _ForkAndThreadTest
global ForkAndThreadTest

_ForkAndThreadTest:
ForkAndThreadTest:
  mov rdi, 2
  mov rsi, NewProgram
  syscall
.waitDead:
  mov rdi, 12
  syscall
  cmp rax, 2
  je .done
  jmp .waitDead
.done:
  mov rdi, 13
  syscall
  cmp rax, 2
  jne .failure
.success:
  mov rax, 1
  ret
.failure:
  xor rax, rax
  ret

NewProgram:
  mov rdi, 1
  mov rsi, .newThread
  syscall
.hang:
  jmp .hang
.newThread:
  mov rdi, 3
  syscall
