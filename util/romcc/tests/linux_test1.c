#include "linux_syscall.h"

static void main(void)
{
	static const char msg[] = "hello world\r\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	_exit(0);
}
