MEMORY
{
/* Start addresses and lengths from the reference manual, section 2.3.2 */
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM (rwx) : ORIGIN = 0x24000000, LENGTH = 128K
}

SECTIONS
{
    /* Interrupt service routine vector table at the start of flash memory. */
    /* In this case only the CPU reset is handled, by directly calling the main function. */
    .isr_vector :
    { *(.isr_vector*) } > FLASH

    /* The .text section of the executable contains the code. */
    .text : {
        *(.text*)
    } > FLASH

    /* /\* Not necessary because we have no initialized global variables. *\/ */
    /* .data : { */
    /*     *(.data*) */
    /* } > SRAM */

    /* /\* Not necessary because we have no uninitialized global variables. *\/ */
    /* .bss : { */
    /*     *(.bss*) */
    /* } > SRAM */
}

/* Define the top of stack as the end of RAM. Stacks grow downwards. */
top_of_stack = ORIGIN(SRAM) + LENGTH(SRAM);
