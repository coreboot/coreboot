#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "southbridge/amd/cs5535/cs5535_early_smbus.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

//#include "lib/delay.c"
#include "northbridge/amd/gx2/raminit.h"
#include "northbridge/amd/gx2/raminit.c"
#include "sdram/generic_sdram.c"

static void msr_init(void)
{

	__builtin_wrmsr(0x1808,  0x10f3bf00, 0x22fffc02);
	
	__builtin_wrmsr(0x10000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x10000021, 0x80fffe0, 0x20000000);
        __builtin_wrmsr(0x10000026, 0x400fffc0, 0x2cfbc040);
        __builtin_wrmsr(0x10000027, 0xfff00000, 0xff);
        __builtin_wrmsr(0x10000028, 0x7bf00100, 0x2000000f);
        __builtin_wrmsr(0x1000002c, 0xff030003, 0x20000000);
        __builtin_wrmsr(0x10000080, 0x3, 0x0);

        __builtin_wrmsr(0x40000020, 0xfff80, 0x20000000);
        __builtin_wrmsr(0x40000021, 0x80fffe0, 0x20000000);
	__builtin_wrmsr(0x40000023, 0x400fffc0, 0x20000040);
        __builtin_wrmsr(0x40000024, 0xff4ffffc, 0x200000ef);
        __builtin_wrmsr(0x40000029, 0x7bf00100, 0x2000000f);
        __builtin_wrmsr(0x4000002d, 0xff030003, 0x20000000);

        __builtin_wrmsr(0x400000e3, 0xf0309c10, 0x0);

        __builtin_wrmsr(0xc0002001, 0x86002, 0x0);
        __builtin_wrmsr(0x80002001, 0x86002, 0x0);
        __builtin_wrmsr(0xa0002001, 0x86002, 0x0);
        __builtin_wrmsr(0x50002001, 0x27, 0x0);
        __builtin_wrmsr(0x4c002001, 0x1, 0x0);
}

static void pll_reset(void)
{
	msr_t msr;

	msr = rdmsr(0x4c000014);
	print_debug("CGLP_SYS_RSTPLL ");
	print_debug_hex32(msr.hi);
	print_debug(":");
	print_debug_hex32(msr.lo);
	print_debug("\n\r");

	if ((msr.lo >> 26) & 0x3F) {
		print_debug("reboot from BIOS reset\n\r");
		return;
	}
	print_debug("prgramming PLL\n\r");
	
	msr.hi = 0x00000019;
	msr.lo = 0x06de0378;
	wrmsr(0x4c000014, msr);
	msr.lo |= ((0xde << 16) | (1 << 26));
	wrmsr(0x4c000014, msr);

	print_debug("Reset PLL\n\r");

	msr.lo |= ((1<<14) |(1<<13) | (1<<0));
	wrmsr(0x4c000014,msr);

	print_debug("should not be here\n\r");


}
static void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	msr_init();

	w83627hf_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	print_err("hi\n\r");

	pll_reset();
	msr_init();

	/* Halt if there was a built in self test failure */
	//report_bist_failure(bist);
	
	sdram_initialize(1, memctrl);
	
	/* Check all of memory */
	ram_check(0x00000000, 1024*1024);

#if 0
	ram_check(0x00000000, msr.lo);

	static const struct {
		unsigned long lo, hi;
	} check_addrs[] = {
		/* Check 16MB of memory @ 0*/
		{ 0x00000000, 0x01000000 },
#if TOTAL_CPUS > 1
		/* Check 16MB of memory @ 2GB */
		{ 0x80000000, 0x81000000 },
#endif
	};
	int i;
	for(i = 0; i < sizeof(check_addrs)/sizeof(check_addrs[0]); i++) {
		ram_check(check_addrs[i].lo, check_addrs[i].hi);
	}
#endif
}
