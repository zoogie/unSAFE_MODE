ENTRY(_start)
SECTIONS
{
    . = 0x08006000;

    __start__ = .;
    .text       : ALIGN(4) { *(.text.start) *(.text*); . = ALIGN(4); }
    .rodata     : ALIGN(4) { *(.rodata*); . = ALIGN(4); }
    .data       : ALIGN(4) { *(.data*); . = ALIGN(8); . = ALIGN(8); __bss_start = .; *(.bss* COMMON); . = ALIGN(8); __bss_end = .;}

    . = ALIGN(32);

    __end__ = .;
}
