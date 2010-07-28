#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include "lib/ramtest.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>
#include <cpu/amd/geode_post_code.h>
#include "southbridge/amd/cs5535/cs5535.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5535/cs5535_early_smbus.c"
#include "southbridge/amd/cs5535/cs5535_early_setup.c"
#include "northbridge/amd/gx2/raminit.h"

/* this has to be done on a per-mainboard basis, esp. if you don't have smbus */
static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	msr_t msr;
	/* 1. Initialize GLMC registers base on SPD values,
	 * Hard coded as XpressROM for now */
	//print_debug("sdram_enable step 1\n");
	msr = rdmsr(0x20000018);
	msr.hi = 0x10076013;
	msr.lo = 0x3400;
	wrmsr(0x20000018, msr);

	msr = rdmsr(0x20000019);
	msr.hi = 0x18000008;
	msr.lo = 0x696332a3;
	wrmsr(0x20000019, msr);

}

#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"

#define PLLMSRhi 0x00000226
#define PLLMSRlo 0x00000008
#define PLLMSRlo1 ((0xde << 16) | (1 << 26) | (1 << 24))
#define PLLMSRlo2 ((1<<14) |(1<<13) | (1<<0))
#include "northbridge/amd/gx2/pll_reset.c"
#include "cpu/amd/model_gx2/cpureginit.c"
#include "cpu/amd/model_gx2/syspreinit.c"
#include "cpu/amd/model_lx/msrinit.c"

void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};
	unsigned char temp;
	SystemPreInit();
	msr_init();

	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	uart_init();
	console_init();

	cs5535_early_setup();
	print_err("done cs5535 early\n");

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();
	print_err("done pll_reset\n");

	cpuRegInit();
	print_err("done cpuRegInit\n");

	sdram_initialize(1, memctrl);

	print_err("Done sdram_initialize\n");
	print_err("Disable watchdog\n");
	outb( 0x87, 0x4E);                            //enter SuperIO configuration mode
	outb( 0x87, 0x4E);

   	outb(0x20, 0x4e);
	temp = inb(0x4f);
	print_debug_hex8(temp);
	if (temp != 0x52){
		print_err("CAN NOT READ SUPERIO VID\n");
	}

	outb(0x29, 0x4e);
	outb(0x7c, 0x4f);

	outb( 0x07, 0x4E);                            //enable logical device 9
	outb( 0x09, 0x4F);
	outb(0x30, 0x4e);
	outb(1, 0x4f);
	outb( 0xF0, 0x4E);                            //set GP33 as outbut in configuration register F0h     Bit4 = \u20180\u2019
	outb( 0xC7, 0x4F);
	outb( 0xF1, 0x4E);                            //clr GP33 (Bit4) value in cofiguration register F1h to \u20181\u2019 disables
	temp = inb(0x4F);                            //watchdog function. Make sure to let the other Bits unchanged!
	print_debug_hex8(temp);print_debug(":");
	temp = temp & ~8;
	outb( temp, 0x4F);
	temp = inb(0x4F);                            //watchdog function. Make sure to let the other Bits unchanged!
	print_debug_hex8(temp);print_debug("\n");
	/* Check all of memory */
//	ram_check(0, 16384);
	ram_check(0x20000, 0x24000);
//	ram_check(0x00000000, 640*1024);

}

