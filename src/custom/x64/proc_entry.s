section .text

.bits 16
global proc_entry_start
proc_entry_start:
  ; initialize long mode by setting the page table root entry

.bits 64
global proc_entry_end
proc_entry_end:
  ; here, we are in long mode and should call back to a kernel method
