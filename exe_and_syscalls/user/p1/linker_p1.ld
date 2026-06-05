ENTRY(_start)

MEMORY
{
    /* P1 vive en su región fija de 64KB según el PDF */
    ram : ORIGIN = DEFINED(PROG_START) ? PROG_START : 0x82100000, LENGTH = 64K
}

SECTIONS
{
    .text : {

        KEEP(*(.text._start)) 
        *(.text*)
        *(.rodata*)
    } > ram

    .data : {
        *(.data*)
    } > ram

    .bss : {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > ram

    /* El stack de P1: El PDF pide 8KB */
    _stack_top = ORIGIN(ram) + LENGTH(ram);
}