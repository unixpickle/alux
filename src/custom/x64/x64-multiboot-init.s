section .text

%include "./addresses.s"
org LOADBASE

bits 32

; these are multiboot flags
MBALIGN equ 1<<0
MEMINFO equ 1<<1
VIDEOINFO equ 1<<2
LINKINFO equ 1<<16
FLAGS equ LINKINFO | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

multiboot_header:
  dd MAGIC
  dd FLAGS
  dd CHECKSUM
  dd multiboot_header ; header pointer
  dd LOADBASE         ; base pointer
  dd 0                ; loadEndAddr - can be 0 to default
  dd 0                ; bssEndAddr, 0 = no BSS segment
  dd start            ; entry point

start:
  mov esp, initial_stack.end
  mov ebp, esp
  
  ; generate page tables
  mov edi, .initialError
  call print_error
.initialError:
  db 'Not yet implemented :(', 0

; move error string to edi
print_error:
  mov ah, 0x0a ; green
  mov edx, 0xb8000
.mainLoop:
  mov al, [edi]
  test al, al
  jz .donePrint
  
  mov [edx], ax
  add ecx, 2
  
  inc edi
  jmp .mainLoop
.donePrint:
  hlt

; static data area

initial_stack:
  times 0x1000 db 0x0
.end:

align 0x1000
initial_pml4:
  dq initial_pdpt + 3
  times 0x1ff dq 0x0

align 0x1000
initial_pdpt:
  dq 0b110000011 ; global, granularity (1GB), writable, user
  times 0x1ff dq 0x0

; pad the result file to 0x100 bytes
times (0x4000 - ($ - multiboot_header)) db 0x0
