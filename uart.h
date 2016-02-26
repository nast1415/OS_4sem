#pragma once
#include "ioport.h"

#define UART_PORT 0x3f8
#define UART_DIVISOR 1

void init_uart();
void puts_uart(const char* string);
