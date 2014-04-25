
SECTIONS {

  . = 0x104000;

  .text BLOCK(16) : ALIGN(16) {
    *(.text)
  }
  
  .rodata BLOCK(16) : ALIGN(16) {
    *(.rodata)
  }
  
  .data BLOCK(16) : ALIGN(16) {
    *(.data)
  }
  
  _bss_size = SIZEOF(*(.bss))

}
