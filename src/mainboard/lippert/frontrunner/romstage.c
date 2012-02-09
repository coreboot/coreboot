#include <stdint.h>
#include <spd.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include "superio/winbond/w83627hf/early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>
#include "southbridge/amd/cs5535/cs5535.h"
#include "southbridge/amd/cs5535/early_smbus.c"
#include "southbridge/amd/cs5535/early_setup.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static const unsigned char spdbytes[] = {	/* 4x Qimonda HYB25DC512160CF-6 */
	0xFF, 0xFF,				/* only values used by raminit.c are set */
	[SPD_MEMORY_TYPE]		= SPD_MEMORY_TYPE_SDRAM_DDR,	/* (Fundamental) memory type */
	[SPD_NUM_ROWS]			= 0x0D,	/* Number of row address bits [13] */
	[SPD_NUM_COLUMNS]		= 0x0A,	/* Number of column address bits [10] */
	[SPD_NUM_DIMM_BANKS]		= 1,	/* Number of module rows (banks) */
	0xFF, 0xFF, 0xFF,
	[SPD_MIN_CYCLE_TIME_AT_CAS_MAX]	= 0x60,	/* SDRAM cycle time (highest CAS latency), RAS access time (tRAC) [6.0 ns in BCD] */
	0xFF, 0xFF,
	[SPD_REFRESH]			= 0x82,	/* Refresh rate/type [Self Refresh, 7.8 us] */
	[SPD_PRIMARY_SDRAM_WIDTH]	= 64,	/* SDRAM width (primary SDRAM) [64 bits] */
	0xFF, 0xFF, 0xFF,
	[SPD_NUM_BANKS_PER_SDRAM]	= 4,	/* SDRAM device attributes, number of banks on SDRAM device */
	[SPD_ACCEPTABLE_CAS_LATENCIES]	= 0x1C,	/* SDRAM device attributes, CAS latency [3, 2.5, 2] */
	0xFF, 0xFF,
	[SPD_MODULE_ATTRIBUTES]		= 0x20,	/* SDRAM module attributes [differential clk] */
	[SPD_DEVICE_ATTRIBUTES_GENERAL]	= 0x40,	/* SDRAM device attributes, general [Concurrent AP] */
	[SPD_SDRAM_CYCLE_TIME_2ND]	= 0x60,	/* SDRAM cycle time (2nd highest CAS latency) [6.0 ns in BCD] */
	0xFF,
	[SPD_SDRAM_CYCLE_TIME_3RD]	= 0x75,	/* SDRAM cycle time (3rd highest CAS latency) [7.5 ns in BCD] */
	0xFF,
	[SPD_tRP]			= 72,	/* Min. row precharge time [18 ns in units of 0.25 ns] */
	[SPD_tRRD]			= 48,	/* Min. row active to row active [12 ns in units of 0.25 ns] */
	[SPD_tRCD]			= 72,	/* Min. RAS to CAS delay [18 ns in units of 0.25 ns] */
	[SPD_tRAS]			= 42,	/* Min. RAS pulse width = active to precharge delay [42 ns] */
	[SPD_BANK_DENSITY]		= 0x40,	/* Density of each row on module [256 MB] */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	[SPD_tRFC]			= 72	/* SDRAM Device Minimum Auto Refresh to Active/Auto Refresh [72 ns] */
};

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	if (device != DIMM0)
		return 0xFF;	/* No DIMM1, don't even try. */

#if CONFIG_DEBUG_SMBUS
	if (address >= sizeof(spdbytes) || spdbytes[address] == 0xFF) {
		print_err("ERROR: spd_read_byte(DIMM0, 0x");
		print_err_hex8(address);
		print_err(") returns 0xff\n");
	}
#endif

	/* Fake SPD ROM value */
	return (address < sizeof(spdbytes)) ? spdbytes[address] : 0xFF;
}

#include "northbridge/amd/gx2/raminit.h"
#include "northbridge/amd/gx2/pll_reset.c"
#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/geode_gx2/cpureginit.c"
#include "cpu/amd/geode_gx2/syspreinit.c"
#include "cpu/amd/geode_lx/msrinit.c"

void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {DIMM0, DIMM1}}
	};
	unsigned char temp;

	SystemPreInit();
	msr_init();

	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
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
}
