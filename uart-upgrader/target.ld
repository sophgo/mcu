/* Entry Point */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(reset)

MEMORY
{
    /* RAM hold all code and data */
    RAM : ORIGIN = 0x20000000, LENGTH = 8K
    /* ROM only hold a picec of code that copy real code to its position */
    /* ROM code should position independent */
    ROM : ORIGIN = 0x0800d000, LENGTH = 8K
}

SECTIONS
{
    /* mini loader, load executable to ram */
    .loader :
    {
        . = ALIGN(4);
        KEEP(*(.loader));
    } > ROM AT > ROM
    /* The startup code goes first into FLASH */
    .vector :
    {
        . = ALIGN(4);
        __ld_program_start = .;
        KEEP(*(.vector)); /* Startup code */
        . = ALIGN(4);
    } > RAM AT > ROM

    __ld_loader_lma = LOADADDR(.loader);
    __ld_vector_lma = LOADADDR(.vector);
    __ld_vector_offset = __ld_vector_lma - __ld_loader_lma;

    .text :
    {
        . = ALIGN(4);
        KEEP(*(.startup)); /* Startup code */
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
        *(.data);           /* .data sections */
        *(.data*);          /* .data* sections */
        . = ALIGN(4);
        __ld_program_end = .;
    } > RAM AT > ROM

    /* Uninitialized data section */
    . = ALIGN(4);
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
    } > RAM AT > ROM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * );
        libm.a ( * );
        libgcc.a ( * );
    }

    __ld_ram_end = ORIGIN(RAM) + LENGTH(RAM);

    .ARM.attributes 0 : { *(.ARM.attributes) }
}


