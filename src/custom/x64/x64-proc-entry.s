bits 16

PROC_ADDR_BASE equ 0x5000

db '_X64_PROC_ENTRY_'
dq (proc_entry_end - proc_entry)
proc_entry:
  cli
  mov sp, PROC_ADDR_BASE - 0x10

  mov eax, 10100000b ; set PAE and PGE bit
  mov cr4, eax
  mov edx, initial_pml4
  mov cr3, edx
  
  ; set the LME and SCE bits of the EFER msr
  mov ecx, 0xC0000080
  rdmsr
  or eax, 0x101
  wrmsr

  ; enable paging and protection simultaneously
  mov ebx, cr0
  or ebx, 0x80000001
  mov cr0, ebx

  mov ebx, gdt_pointer
  lgdt [ebx]
  jmp gdt.code:((proc_entry_64 - proc_entry) + PROC_ADDR_BASE)

bits 64
proc_entry_64:
  pop rax
  mov cr3, rax
  ret

proc_entry_end:
