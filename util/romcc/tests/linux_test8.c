#include "linux_syscall.h"
#include "linux_console.h"

struct mem_param {
	unsigned char cycle_time;
	unsigned char divisor;
	unsigned char tRC;
	unsigned char tRFC;
	unsigned dch_memclk;
	unsigned short dch_tref4k, dch_tref8k;
	unsigned char dtl_twr;
	char name[9];
};

static void test(void)
{
	static const struct mem_param param0 = {
		.name = "166Mhz\r\n",
		.cycle_time = 0x60,
		.divisor = (6<<1),
		.tRC = 0x3C,
		.tRFC = 0x48,
		.dch_memclk = 5 << 20,
		.dch_tref4k = 0x02,
		.dch_tref8k = 0x0A,
		.dtl_twr = 3,
	};
	int value;
	unsigned clocks;
	const struct mem_param *param;
	param = &param0;
	value = 0x48;
	/* This used to generate 32bit loads instead of 8 bit loads */
	clocks = (value + (param->divisor << 1) - 1)/(param->divisor << 1);
	print_debug("clocks: ");
	print_debug_hex32(clocks);
	print_debug("\r\n");
	_exit(0);
}
