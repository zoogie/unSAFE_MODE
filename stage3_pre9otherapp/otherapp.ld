OUTPUT_ARCH(arm)

MEMORY
{
	RAMRO (rx) : ORIGIN = 0x00101000, LENGTH = 0x0000C000
	RAMRW (rw!i) : ORIGIN = 0x8000000, LENGTH = 0x00100000
}

SECTIONS
{
	.text : ALIGN(0x1000) {
		build/crt0.o(.init)
		*(.text)
	}

	.rodata ALIGN(0x4) :
	{
		*(.rodata)
		*(.roda)
		*(.rodata.*)
		*all.rodata*(*)
		*(.gnu.linkonce.r*)
		SORT(CONSTRUCTORS)
		. = ALIGN(4);
	}
	
	.bss : {
		_bss_start = .;
		*(.bss);
	}
	_bss_end = .;
	
}