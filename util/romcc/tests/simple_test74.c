struct syscall_result {
	long val;
	int errno;
};

static struct syscall_result syscall_return(long result)
{
	struct syscall_result res;
	if (((unsigned long)result) >= ((unsigned long)-125)) {
		res.errno = - result;
		res.val = -1;
	} else {
		res.errno = 0;
		res.val = result;
	}
	return res;
}

static struct syscall_result syscall1(unsigned long nr, unsigned long arg1)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1));
	return syscall_return(res);

}


static struct syscall_result syscall3(unsigned long nr, unsigned long arg1, unsigned long arg2,
	unsigned long arg3)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1), "c" (arg2), "d" (arg3));
	return syscall_return(res);

}

#define NR_exit                 1
#define NR_write                4

/* Standard file descriptors */
#define STDIN_FILENO    0  /* Standard input */
#define STDOUT_FILENO   1  /* Standard output */
#define STDERR_FILENO   2  /* Standard error output */

typedef long ssize_t;
typedef unsigned long size_t;

static ssize_t write(int fd, const void *buf, size_t count)
{
	struct syscall_result res;
	res = syscall3(NR_write, fd, (unsigned long)buf, count);
	return res.val;
}

static void _exit(int status)
{
	struct syscall_result res;
	res = syscall1(NR_exit, status);
}

static void console_tx_string(const char *str)
{
	unsigned char ch;
	while(1) {

	}
	for(;1;) {
	}
	do {
	} while(1);
	if (1) {
	}else {
	}
}


static void main(void)
{
	static const char msg[] = "hello world\r\n";
	write(STDOUT_FILENO, msg, sizeof(msg));
	_exit(0);
}
