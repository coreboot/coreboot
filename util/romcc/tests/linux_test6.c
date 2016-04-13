#include "linux_syscall.h"
#include "linux_console.h"

static void main(void)
{
	static const int value[] = { 1, 0 };
	const char *str;
	if (value[1]) {
		print_debug("A\n");
		str = "Unbuffered\n";
	} else {
		print_debug("B\n");
		str = "Registered\n";
	}
	print_debug(str);
	_exit(0);
}
