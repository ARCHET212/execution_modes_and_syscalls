ENTRY(_start)

MEMORY
{
    /* QEMU carga el kernel en 0x10000 con -kernel. Le damos 8MB por seguridad */
    ram : ORIGIN = 0x00010000, LENGTH = 8M 
}

SECTIONS
{
    . = ORIGIN(ram);
    .text :
    {
        *(.text*)
        *(.rodata*)
    } > ram

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > ram

    .bss :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > ram

    _stack_bottom = __bss_end__;
    _stack_top = ORIGIN(ram) + LENGTH(ram);
}