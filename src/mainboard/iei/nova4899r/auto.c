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
//#include "southbridge/intel/i440bx/i440bx_early_smbus.c"
#include "superio/winbond/w83977tf/w83977tf_early_serial.c"
//#include "northbridge/intel/i440bx/raminit.h"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

//#include "debug.c"
//#include "lib/delay.c"

#include "northbridge/amd/gx1/raminit.c"

static void main(unsigned long bist)
{
	w83977tf_enable_serial(SERIAL_DEV, TTYS0_BASE);
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
	for(i = 0; i < sizeof(check_addrs)/sizeof(check_addrs[0]); i++) {
		ram_check(check_addrs[i].lo, check_addrs[i].hi);
	}
#endif
}
