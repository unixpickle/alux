section .text

bits 64

db '_X64_ENTRYPOINT!'
global x64_entrypoint
x64_entrypoint:
  ; here, we will be in long mode and everything!
