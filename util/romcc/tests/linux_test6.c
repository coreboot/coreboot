#include "linux_syscall.h"
#include "linux_console.h"

static void main(void)
{
	static const int value[] = { 1, 0 };
	const char *str;
	if (value[1]) {
		print_debug("A\r\n");
		str = "Unbuffered\r\n";
	} else {
		print_debug("B\r\n");
		str = "Registered\r\n";
	}
	print_debug(str);
	_exit(0);
}
