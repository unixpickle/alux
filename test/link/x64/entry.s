section .text

extern _main
extern _ThreadExit

global _Entry
_Entry:
  mov rsp, initial_stack_end
  call _main
  call _ThreadExit

section .data

align 16

global initial_stack
global initial_stack_end
initial_stack:
  times 0x4000 db 0
initial_stack_end:
