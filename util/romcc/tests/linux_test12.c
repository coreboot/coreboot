#include "linux_syscall.h"
#include "linux_console.h"

#define MACRO(hello, hello2) 1

#ifndef A
#define A 135
#endif
#define B A
#define C B
#define D C
#define E D
#define F E
#define G F
#define H G
#define I H

#define FOO() "hah?\n"
#define BAR(X) ( X " There\n" )
#define BAZ(X) #X
#define SUM(X, Y) ((X) + (Y))
#define REALLY_SUM(...) SUM(__VA_ARGS__)


#define hash_hash #   	/* comment */	 ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)

#define ECHO(X) X
#define print_debug(X) ECHO(print_debug(X))

static void test(void)
{
	print_debug(FOO());
	print_debug(BAR("Hi!!"));
	print_debug(BAZ(This should be shown as a string... "enclosed in quotes") "\n");
	print_debug("This is a quote\" see\n");
	print_debug(BAR(BAZ(I)));

	print_debug_hex32(REALLY_SUM(1,2));
	print_debug("\n");

	print_debug(join(x, y) "\n");

	print_debug("romcc: ");
	print_debug_hex8(__ROMCC__);
	print_debug(".");
	print_debug_hex8(__ROMCC_MINOR__);
	print_debug("\n");

	print_debug(__FILE__);
	print_debug(":");
	print_debug(__func__);
	print_debug(":");
	print_debug_hex32(__LINE__);
	print_debug("\n");

	print_debug("Compiled at: ");
	print_debug(__DATE__);
	print_debug(" ");
	print_debug(__TIME__);
	print_debug("\n");

	print_debug("Compile time: ");
	print_debug(__TIME__);
	print_debug("\n");

	_exit(0);
}
