#include "linux_syscall.h"
#include "linux_console.h"

static void test(void)
{
	signed char x;
	x = -1;
	print_debug_hex32(x);
	print_debug("\n");
	_exit(0);
}
