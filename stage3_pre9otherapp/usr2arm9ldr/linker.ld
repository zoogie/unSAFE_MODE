ENTRY(_start)
SECTIONS
{
    . = 0;

    .text       : ALIGN(4) { *(.text.start) *(.text*); . = ALIGN(4); }
    .rodata     : ALIGN(4) { *(.rodata*); *(.got*); . = ALIGN(4); }
    .data       : ALIGN(4) { *(.data*); . = ALIGN(8); . = ALIGN(8); __bss_start = .; *(.bss* COMMON); . = ALIGN(8); __bss_end = .;}

    . = ALIGN(4);
}
