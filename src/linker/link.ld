ENTRY(_init)
SECTIONS
{
    . = 0x00500000;
    .text :
    {
        *(.text)
        . = ALIGN(4096);
        _kernel_text_end = .;
    }
    .data :
    {
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
        _kernel_data_end = .;
    }
    .bss :
    {
        *(.bss)
        . = ALIGN(4096);
        _kernel_bss_end = .;
    }
    /DISCARD/ : { *(.comment) *(.eh_frame) }
}
. = ALIGN(4096);
_kernel_end = .;
