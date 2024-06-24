#include <stdint.h>

/* Base addresses for RCC and GPIOB */
#define RCC_BASE        0x58024400
#define GPIOB_BASE      0x58020400

/* Register offsets */
#define RCC_AHB4ENR     (*(volatile uint32_t *)(RCC_BASE + 0xE0))
#define GPIOB_MODER     (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x14))

/* LED pin */
#define LED_PIN         0
#define LED_ON          (1 << LED_PIN)
#define LED_OFF         (0 << LED_PIN)

/* Simple delay function */
void delay(volatile uint32_t count) {
    while (count--) {
        /* no-op to prevent the compiler from optimizing out this loop */
        __asm__ volatile ("nop");
    }
}

int main(void) {
    /* Enable GPIOB clock */
    RCC_AHB4ENR |= (1 << 1);

    /*
     * Set GPIOB_PIN0 as output
     * First, reset the relevant bits to 0, then set them to 0b01
     */
    GPIOB_MODER &= ~(3 << (LED_PIN * 2));  /* Clear mode bits for pin 0 */
    GPIOB_MODER |= (1 << (LED_PIN * 2));   /* Set mode to output */

    while (1) {
        /* Toggle LED */
        GPIOB_ODR ^= LED_ON;
        delay(10000000);
    }
}
