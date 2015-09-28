#include <stdio.h>
#include <stdtypes.h>
#include "testbios.h"

int int15_handler(void)
{
	printf("\nint15 encountered.\n");
	x86emu_dump_xregs();
	X86_EAX = 0;
	return 1;
}
