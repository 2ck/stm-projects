#include <stdint.h>

extern int main(void);
extern uint32_t top_of_stack;

// Minimal vector table
__attribute__((section(".isr_vector")))
uint32_t *vector_table[] = {
    &top_of_stack,      // Initial stack pointer, defined in the linker script
    (uint32_t *)main    // Reset vector: Directly calls main on startup
};
