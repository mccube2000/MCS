ENTRY(_MSC_main)
SECTIONS
{
	. = 0x00280000;
	.text :
	{
		*(.text)
		. = ALIGN(16);
	}
	.data :
	{
		*(.data)
		*(.rodata)
		. = ALIGN(16);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(16);
	}
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}