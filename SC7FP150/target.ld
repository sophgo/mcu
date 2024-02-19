OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Reset_Handler)
EXTERN(exception_table)

MEMORY
{
    RAM : ORIGIN = 0x20000000, LENGTH = 96K
    ROM : ORIGIN = 0x08000000, LENGTH = 256K
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        __ld_program_start = .;
        KEEP(*(.vector));   /* startup code */
        *(.text.crt_init);  /* make sure crt_init located into 8K offset */
        *(.text);           /* .text sections (code) */
        *(.text*);          /* .text* sections (code) */
        *(.glue_7);         /* glue arm to thumb code */
        *(.glue_7t);        /* glue thumb to arm code */
        *(.eh_frame);

        KEEP (*(.init));
        KEEP (*(.fini));
        . = ALIGN(4);
    } > RAM AT > ROM

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata);         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*);        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > RAM AT > ROM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*);
    } > RAM AT > ROM

    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*);
        __exidx_end = .;
    } > RAM AT > ROM

    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*));
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > RAM AT > ROM
    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)));
        KEEP (*(.init_array*));
        PROVIDE_HIDDEN (__init_array_end = .);
    } > RAM AT > ROM
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)));
        KEEP (*(.fini_array*));
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > RAM AT > ROM

    .note :
    {
        *(.note.*);
    } > RAM AT > ROM

    /* Initialized data sections goes into RAM, load LMA copy after code */
    .data : 
    {
        . = ALIGN(4);
        __ld_data_start = .;
        *(.data);           /* .data sections */
        *(.data*);          /* .data* sections */
        . = ALIGN(4);
        __ld_data_end = .;
        __ld_program_end = .;
    } > RAM AT > ROM

    __ld_program_load_start = LOADADDR(.text);
    __ld_data_load_start = LOADADDR(.data);
    __ld_data_load_end = LOADADDR(.data) + SIZEOF(.data);
    __ld_reset_handler = __ld_program_load_start +
        (Reset_Handler - __ld_program_start);

    /* Uninitialized data section */
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        . = ALIGN(4);
        __ld_bss_start = .;
        *(.bss);
        *(.bss*);
        *(COMMON);
        . = ALIGN(4);
        __ld_bss_end = .;
    } > RAM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * );
        libm.a ( * );
        libgcc.a ( * );
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }

    __ld_ram_end = ORIGIN(RAM) + LENGTH(RAM);
    __ld_stack_top = __ld_ram_end;
}


