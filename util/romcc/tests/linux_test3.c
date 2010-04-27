#include "linux_syscall.h"
#include "linux_console.h"
static void goto_test(void)
{
	int i;
	print_debug("goto_test\n");

	i = 0;
	goto bottom;
	{
	top:
		print_debug("i = ");
		print_debug_hex8(i);
		print_debug("\n");

		i = i + 1;
	}
 bottom:
	if (i < 10) {
		goto top;
	}
}

static void main(void)
{
	goto_test();
	_exit(0);
}
