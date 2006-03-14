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
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5535/cs5535_early_smbus.c"
#include "southbridge/amd/cs5535/cs5535_early_setup.c"
#include "northbridge/amd/gx2/raminit.h"

static void sdram_set_spd_registers(const struct mem_controller *ctrl) {
	msr_t msr;
	/* 1. Initialize GLMC registers base on SPD values,
	 * Hard coded as XpressROM for now */
	//print_debug("sdram_enable step 1\r\n");
	msr = rdmsr(0x20000018);
	msr.hi = 0x10076013;
	msr.lo = 0x00003000;
	wrmsr(0x20000018, msr);

	msr = rdmsr(0x20000019);
	msr.hi = 0x18000108;
	msr.lo = 0x696332a3;
	wrmsr(0x20000019, msr);

	
}
#include "northbridge/amd/gx2/raminit.c"
#include "sdram/generic_sdram.c"

#include "northbridge/amd/gx2/pll_reset.c"

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


        __builtin_wrmsr(0x50002001, 0x27, 0x0);
        __builtin_wrmsr(0x4c002001, 0x1, 0x0);
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

	cs5535_early_setup();

	pll_reset();

	/* Halt if there was a built in self test failure */
	//report_bist_failure(bist);
	
	sdram_initialize(1, memctrl);

	
	/* Check all of memory */
	ram_check(0x00000000, 640*1024);

}
