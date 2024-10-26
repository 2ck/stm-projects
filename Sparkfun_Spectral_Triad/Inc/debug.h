#include <stdarg.h>  // Needed for variadic functions
#include <stdint.h>
#include "stm32h7xx_hal.h"

extern UART_HandleTypeDef huart3;

static inline void print(const char *format, ...) {
	va_list args;
	va_start(args, format);

	while (*format) {
		if (*format == '%') {
			format++;  // Move to the format specifier
			switch (*format) {
				case 'd': {  // Print integer
					uint32_t num = va_arg(args, uint32_t);
					char buffer[11];  // Buffer to store the number as a string
					int len = 0;
					do {
						buffer[len++] = (char)((num % 10) + '0');  // Get each digit
						num /= 10;
					} while (num > 0);

					// Reverse and print the number
					for (int i = len - 1; i >= 0; i--) {
						HAL_UART_Transmit(&huart3, (uint8_t*)&buffer[i], 1, 1000);
					}
					break;
				}
				case 'c': {  // Print character
					char c = (char)va_arg(args, int);  // Get character from arguments
					HAL_UART_Transmit(&huart3, (uint8_t*)&c, 1, 1000);
					break;
				}
				case 's': {  // Print string
					const char *str = va_arg(args, const char*);
					while (*str) {
						HAL_UART_Transmit(&huart3, (uint8_t*)str++, 1, 1000);
					}
					break;
				}
				case 'f': {  // Print float
					double num = va_arg(args, double);  // Get the float argument
					print("%d.", (uint32_t)num);
					// print_float(num, 2);  // Print float with 2 decimal places
					double frac_part = num - (uint32_t)num;
					uint8_t precision = 3;
					for (int i = 0; i < precision; i++) {
						frac_part *= 10;
						print("%d", (uint32_t)frac_part);
						frac_part -= (uint32_t)frac_part;
					}
					break;
				}
				default:
					break;
			}
		} else {
			// Print regular character if not a format specifier
			HAL_UART_Transmit(&huart3, (uint8_t*)format, 1, 1000);
		}
		format++;
	}

	va_end(args);
}
