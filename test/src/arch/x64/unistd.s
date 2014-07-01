section .text

extern _ThreadExit
extern initial_stack_end

global _fork
global fork

_fork:
fork:
  mov rdx, rdi
  mov rsi, forkEntry
  mov rdi, 2
  syscall
  ret

db 'hey there noob'
forkEntry:
  ; TODO: delete this
  mov rsi, rdi
  mov rdi, 0x1337
  syscall

  mov rsp, initial_stack_end
  call rdi
  call qword _ThreadExit

global _getpid
global getpid
