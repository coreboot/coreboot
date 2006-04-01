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
#include "superio/NSC/pc87360/pc87360_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>

#define SERIAL_DEV PNP_DEV(0x2e,  PC87360_SP1)

#include "southbridge/amd/cs5535/cs5535_early_smbus.c"
#include "southbridge/amd/cs5535/cs5535_early_setup.c"
#include "northbridge/amd/gx2/raminit.h"

/* this has to be done on a per-mainboard basis, esp. if you don't have smbus */
static void sdram_set_spd_registers(const struct mem_controller *ctrl) 
{
	msr_t msr;
	/* 1. Initialize GLMC registers base on SPD values,
	 * Hard coded as XpressROM for now */
	//print_debug("sdram_enable step 1\r\n");
	msr = rdmsr(0x20000018);
	msr.hi = 0x10075012;
	msr.lo = 0x3900;
	wrmsr(0x20000018, msr);

	msr = rdmsr(0x20000019);
	msr.hi = 0x18000008;
	msr.lo = 0x696332a3;
	wrmsr(0x20000019, msr);

}

#include "northbridge/amd/gx2/raminit.c"
#include "sdram/generic_sdram.c"

#define PLLMSRhi 0x00000226
#define PLLMSRlo 0x00000008
#define PLLMSRlo1 ((0xde << 16) | (1 << 26) | (1 << 24))
#define PLLMSRlo2 ((1<<14) |(1<<13) | (1<<0))
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
	unsigned char temp;

	msr_init();

	pc87360_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	cs5535_early_setup();
	print_err("done cs5535 early\n");
	pll_reset();
	print_err("done pll_reset\n");
	/* Halt if there was a built in self test failure */
	//report_bist_failure(bist);
	
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
//	ram_check(0x20000, 0x24000);
//	ram_check(0x00000000, 640*1024);

}
