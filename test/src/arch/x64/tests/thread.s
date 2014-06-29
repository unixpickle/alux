section .data

doneFlag:
  db 0x0

align 8

countField:
  dq 0x0

argumentField:
  dq 0x0

section .text

global _BasicThreadTest
global BasicThreadTest

_BasicThreadTest:
BasicThreadTest:
  mov rdi, 1
  mov rsi, .newThread
  mov rdx, 0x1337
  syscall
.loop:
  mov rcx, doneFlag
  mov al, [rcx]
  test al, al
  jnz .loopEnd
  jmp .loop
.loopEnd:
  ; verify fields
  mov rcx, argumentField
  cmp qword [rcx], 0x1337
  jne .fail
  mov rcx, countField
  cmp qword [rcx], 3 ; GarbageThread + main thread + new thread
  jne .fail
.waitDealloc:
  mov rdi, 13
  syscall
  cmp rax, 2
  je .success
  jmp .waitDealloc
.fail:
  xor rax, rax
  ret
.success:
  mov rax, 1
  ret

; the thread that gets launched
.newThread:
  mov rcx, argumentField
  mov [rcx], rdi
  mov rdi, 13
  syscall
  mov rcx, countField
  mov [rcx], rax
  mov rcx, doneFlag
  mov byte [rcx], 1
  mov rdi, 4
  syscall
