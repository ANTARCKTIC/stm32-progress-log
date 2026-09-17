ENTRY(Reset_Handler)

MEMORY
{
    flash (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    sram  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(sram) + LENGTH(sram);

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > flash

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(4);
        _etext = .;
    } > flash

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
    } > flash

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > flash

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > flash

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.first_data)
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } > sram AT > flash

    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > sram

    . = ALIGN(8);
    _end = .;
}