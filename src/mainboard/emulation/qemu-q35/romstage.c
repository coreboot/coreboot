/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <device/pci_ops.h>

#include "q35.h"

void mainboard_romstage_entry(void)
{
	i82801ix_early_init();

	if (!CONFIG(BOOTBLOCK_CONSOLE))
		mainboard_machine_check();

	cbmem_recovery(0);
}
