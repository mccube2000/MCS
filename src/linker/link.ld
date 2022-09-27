ENTRY(_MSC_main)
SECTIONS
{
	. = 0x00040000;
	.text :
	{
		*(.text)
		. = ALIGN(4096);
	}
	.data :
	{
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
	}
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}