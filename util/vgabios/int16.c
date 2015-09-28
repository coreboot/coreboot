#include <stdio.h>
#include "testbios.h"

int int16_handler(void)
{
	printf("\nint16: keyboard not supported right now.\n");
	x86emu_dump_xregs();
	return 1;
}
