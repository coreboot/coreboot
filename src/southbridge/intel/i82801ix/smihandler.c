/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_def.h>
#include <southbridge/intel/common/pmutil.h>
#include "i82801ix.h"

#include <soc/nvs.h>

#if CONFIG(SMM_LEGACY_ASEG)
/* For qemu/x86-q35 to build properly. */
struct global_nvs *gnvs;
#endif

int southbridge_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x32:
		printk(BIOS_DEBUG, "OS Init\n");
		/* gnvs->smif:
		 *  On success, the IO Trap Handler returns 0
		 *  On failure, the IO Trap Handler returns a value != 0
		 */
		gnvs->smif = 0;
		return 1; /* IO trap handled */
	}

	/* Not handled */
	return 0;
}

void southbridge_smi_monitor(void)
{
#define IOTRAP(x) (trap_sts & (1 << x))
	u32 trap_sts, trap_cycle;
	u32 data, mask = 0;
	int i;

	trap_sts = RCBA32(0x1e00); // TRSR - Trap Status Register
	RCBA32(0x1e00) = trap_sts; // Clear trap(s) in TRSR

	trap_cycle = RCBA32(0x1e10);
	for (i=16; i<20; i++) {
		if (trap_cycle & (1 << i))
			mask |= (0xff << ((i - 16) << 3));
	}

	/* IOTRAP(3) SMI function call */
	if (IOTRAP(3)) {
		if (gnvs && gnvs->smif)
			io_trap_handler(gnvs->smif); // call function smif
		return;
	}

	/* IOTRAP(2) currently unused
	 * IOTRAP(1) currently unused */

	/* IOTRAP(0) SMIC */
	if (IOTRAP(0)) {
		if (!(trap_cycle & (1 << 24))) { // It's a write
			printk(BIOS_DEBUG, "SMI1 command\n");
			data = RCBA32(0x1e18);
		}
		// Fall through to debug
	}

	printk(BIOS_DEBUG, "  trapped io address = 0x%x\n", trap_cycle & 0xfffc);
	for (i=0; i < 4; i++) if (IOTRAP(i)) printk(BIOS_DEBUG, "  TRAP = %d\n", i);
	printk(BIOS_DEBUG, "  AHBE = %x\n", (trap_cycle >> 16) & 0xf);
	printk(BIOS_DEBUG, "  MASK = 0x%08x\n", mask);
	printk(BIOS_DEBUG, "  read/write: %s\n", (trap_cycle & (1 << 24)) ? "read" : "write");

	if (!(trap_cycle & (1 << 24))) {
		/* Write Cycle */
		data = RCBA32(0x1e18);
		printk(BIOS_DEBUG, "  iotrap written data = 0x%08x\n", data);
	}
#undef IOTRAP
}

void southbridge_finalize_all(void)
{
}
