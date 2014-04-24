bits 32

section .multiboot
; here, put the multiboot header

section .text

.bits 64
global long_mode_entry
long_mode_entry:
  ; here, call a kernel entry method
