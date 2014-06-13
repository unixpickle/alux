section .text

global _RawSyscallHandler
global RawSyscallHandler
extern _SyscallMain

RawSyscallHandler:
  mov r10, rsp
  mov rsp, [gs:0x10]
  push r10
  
  call _SyscallMain
  
  pop r10
  mov r10, rsp
  sti ; STI doesn't take effect until *after* its following instruction
  sysret

_RawSyscallHandler:
  jmp RawSyscallHandler
