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
#include "southbridge/intel/i82371eb/i82371eb_early_smbus.c"
#include "superio/nsc/pc87351/pc87351_early_serial.c"
#include "northbridge/intel/i440bx/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, PC87351_SP1)

void udelay(int usecs) 
{
	int i;
	for(i = 0; i < usecs; i++)
		outb(i&0xff, 0x80);
}

#include "debug.c"
#include "lib/delay.c"

static void enable_shadow_ram(void) 
{
	uint8_t shadowreg;
	/* dev 0 for northbridge */
	shadowreg = pci_read_config8(0, 0x59);
	/* 0xf0000-0xfffff */
	shadowreg |= 0x30;
	pci_write_config8(0, 0x59, shadowreg);
}

/* TODO: fix raminit.c to use smbus_read_byte */
static inline int spd_read_byte(unsigned device, unsigned address)
{
	uint8_t c;
	c = smbus_read_byte(device, address);
	return c;
}

#include "northbridge/intel/i440bx/raminit.c"
#include "northbridge/intel/i440bx/debug.c"
#include "sdram/generic_sdram.c"

static void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{
		 .d0 = PCI_DEV(0, 0, 0),
		 .channel0 = {
			      (0xa << 3) | 0,
			      (0xa << 3) | 1,
			      (0xa << 3) | 2,
			      (0xa << 3) | 3,
			      },
		 }
	};
	
	if (bist == 0) {
		early_mtrr_init();
	}
	pc87351_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	enable_shadow_ram();

	enable_smbus();

	dump_spd_registers(&memctrl[0]);

	sdram_initialize(sizeof(memctrl) / sizeof(memctrl[0]), memctrl);

	/* Check whether RAM is working.
	 *
	 * Do _not_ check the area from 640 KB - 1 MB, as that's not really
	 * RAM, but rather reserved for various other things:
	 *
	 *  - 640 KB - 768 KB: Video Buffer Area
	 *  - 768 KB - 896 KB: Expansion Area
	 *  - 896 KB - 960 KB: Extended System BIOS Area
	 *  - 960 KB - 1 MB:   Memory (BIOS Area) - System BIOS Area
	 *
	 * Trying to check these areas will fail.
	 */
	/* TODO: This is currently hardcoded to check 64 MB. */
	ram_check(0x00000000, 0x0009ffff);	/* 0 - 640 KB */
	ram_check(0x00100000, 0x007c0000);	/* 1 MB - 64 MB */
}
