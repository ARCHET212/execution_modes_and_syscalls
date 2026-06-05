/* linker_p2.ld corregido */
ENTRY(_start)

MEMORY
{
    /* P2 vive en su región fija de 64KB (2MB después del inicio del OS) */
    ram : ORIGIN = DEFINED(PROG_START) ? PROG_START : 0x82200000, LENGTH = 64K 
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

    /* El stack de P2: 8KB al final de su región */
    _stack_top = ORIGIN(ram) + LENGTH(ram);
}