section .text

extern _main

global _Entry
_Entry:
  mov rsp, initial_stack.end
  call _main
.loop:
  jmp .loop

section .data

align 16

initial_stack:
  times 0x4000 db 0
.end:
