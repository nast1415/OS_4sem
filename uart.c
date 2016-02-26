#include "uart.h"

void init_uart() {
	out8(UART_PORT + 3, 3 | (1 << 7));
	out8(UART_PORT + 0, UART_DIVISOR & 0xff);
	out8(UART_PORT + 1, (UART_DIVISOR >> 8) & 0xff);
	out8(UART_PORT + 3, 3);
	out8(UART_PORT + 1, 0);
}

void puts_uart(const char* str) {
	while (*(str) != 0) {
		while (!(in8(UART_PORT + 5) & (1 << 5)));
		out8(UART_PORT + 0, *str);
		str++;
	}
}