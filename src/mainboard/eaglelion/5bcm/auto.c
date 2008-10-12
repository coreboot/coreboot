#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
//#include "southbridge/intel/i440bx/i440bx_early_smbus.c"
#include "superio/nsc/pc97317/pc97317_early_serial.c"
//#include "northbridge/intel/i440bx/raminit.h"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, PC97317_SP1)

//#include "lib/delay.c"

#include "northbridge/amd/gx1/raminit.c"

static void main(unsigned long bist)
{
	pc97317_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	
	sdram_init();
	
	/* Check all of memory */
#if 0
	ram_check(0x00000000, msr.lo);
#endif
#if 0
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
	for(i = 0; i < ARRAY_SIZE(check_addrs); i++) {
		ram_check(check_addrs[i].lo, check_addrs[i].hi);
	}
#endif
}
