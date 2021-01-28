/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <device/pci_ops.h>

#include "q35.h"

static void mainboard_machine_check(void)
{
	/* Check that MCFG is active. If it's not qemu was started for machine PC */
	if (!CONFIG(BOOTBLOCK_CONSOLE) &&
	    (pci_read_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO) !=
	     (CONFIG_MMCONF_BASE_ADDRESS | 1)))
		die("You must run qemu for machine Q35 (-M q35)");
}

void mainboard_romstage_entry(void)
{
	i82801ix_early_init();

	mainboard_machine_check();

	cbmem_recovery(0);
}
