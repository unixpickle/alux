SECTIONS {

  . = 0x8000000000;

  .text BLOCK(16) : ALIGN(16) {
    *(.text)
  }
  
  .bss BLOCK(16) : ALIGN(16) {
    *(.bss)
  }
  
  .rodata BLOCK(16) : ALIGN(16) {
    *(.rodata)
  }
  
  .data BLOCK(16) : ALIGN(16) {
    *(.data)
    *(COMMON)
    *(.bss)
  }
  
}
