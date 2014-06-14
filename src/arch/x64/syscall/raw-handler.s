section .text

global _RawSyscallHandler
global RawSyscallHandler
extern _SyscallMain

RawSyscallHandler:
  mov r10, rsp
  mov rsp, [gs:0x10]
  push r10
  push rcx
  mov rcx, rbx
  
  call _SyscallMain
  
  pop rcx
  pop rsp
  sti ; STI doesn't take effect until *after* its following instruction
  o64 sysret

_RawSyscallHandler:
  jmp RawSyscallHandler
