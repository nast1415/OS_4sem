#include "ioport.h"
#include "pic.h"
#include "uart.h"


void main(void) {
	init_uart();
	puts_uart("uart initialized\n");
	//init_pic();
	//puts_uart("pic set up\n");
}
