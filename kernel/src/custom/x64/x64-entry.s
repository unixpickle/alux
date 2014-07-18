section .text

bits 64

extern AluxMainX64

db '_X64_ENTRYPOINT!'

global x64_entrypoint
x64_entrypoint:
  ; here, we have been switched into long mode and we have 1GB identity mapped.
  pop rdi
  call AluxMainX64
  cli
  hlt

