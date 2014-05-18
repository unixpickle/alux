bits 16

PROC_ADDR_BASE equ 0x5000

align 16
db '_X64_PROC_ENTRY_'
dq (proc_entry_end - proc_entry)
proc_entry:
  jmp 0:(.start - proc_entry + PROC_ADDR_BASE)
.start:
  cli
  mov sp, PROC_ADDR_BASE - 0x10

  mov eax, 10100000b ; set PAE and PGE bit
  mov cr4, eax
  mov edx, initial_pml4
  mov cr3, edx
  
  ; set the LME, SCE and NXE bits of the EFER msr
  mov ecx, 0xC0000080
  rdmsr
  or eax, 0x901
  wrmsr

  ; enable paging and protection simultaneously
  mov ebx, cr0
  or ebx, 0x80000001
  mov cr0, ebx
  
  mov ebx, local_gdt_ptr - proc_entry + PROC_ADDR_BASE
  lgdt [ebx]
  jmp gdt.code:((proc_entry_64 - proc_entry) + PROC_ADDR_BASE)

align 8
local_gdt_ptr:
  dw gdt_pointer - gdt - 1
  dq gdt

bits 64
proc_entry_64:
  mov ax, gdt.data
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; if I don't set SS=0, iret causes a #GP
  xor ax, ax
  mov ss, ax
  
  hlt

  pop rax
  mov cr3, rax
  ret

proc_entry_end:
