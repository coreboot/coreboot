#include "linux_syscall.h"
#include "linux_console.h"

static void test(void)
{
	unsigned char i;
	for(i = 127; i != 5; i++) {
		print_debug("i: ");
		print_debug_hex32((unsigned )i);
		print_debug("\n");
	}
	_exit(0);
}
