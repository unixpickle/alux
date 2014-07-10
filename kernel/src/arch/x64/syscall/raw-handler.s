section .text

global _RawSyscallHandler
global RawSyscallHandler
extern _SyscallMain

_RawSyscallHandler:
RawSyscallHandler:
  swapgs
  mov r10, rsp
  mov rsp, [gs:0x10]
  push r10
  push rcx
  mov rcx, rbx
  
  call _SyscallMain
  
  pop rcx
  pop rsp
  
  mov r11, 0x200
  
  ; TODO: here, verify that RCX is canonical! Serious security holes could
  ; be created if I do not get around to this!
  
  swapgs
  o64 sysret
