#include "linux_syscall.h"
#include "linux_console.h"

struct stuff {
	signed int   a : 5;
	signed int   b : 6;
	signed int   c : 2;
	unsigned int d : 3;
};

static void test(void)
{
	struct stuff var;
#if 0
	int a, b, c, d;

	a = 1;
	b = 2;
	c = 3;
	d = 7;

	var.a = a;
	var.b = b;
	var.c = c;
	var.d = d;

	a = var.a;
	b = var.b;
	c = var.c;
	d = var.d;

	print_debug(" a: ");
	print_debug_hex32(a);
	print_debug(" b: ");
	print_debug_hex32(b);
	print_debug(" c: ");
	print_debug_hex32(c);
	print_debug(" d: ");
	print_debug_hex32(d);
#else
	var.a = 1;
	var.b = 2;
	var.c = 3;
	var.d = 7;

	print_debug(" a: ");
	print_debug_hex32(var.a);
	print_debug(" b: ");
	print_debug_hex32(var.b);
	print_debug(" c: ");
	print_debug_hex32(var.c);
	print_debug(" d: ");
	print_debug_hex32(var.d);
#endif
	print_debug("\n");
	_exit(0);
}
