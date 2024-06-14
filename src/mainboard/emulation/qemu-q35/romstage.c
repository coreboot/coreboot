/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <device/pci_ops.h>

#include "q35.h"

#define TSEG_SZ_MASK	(3 << 1)

void mainboard_romstage_entry(void)
{
	i82801ix_early_init();

	if (!CONFIG(BOOTBLOCK_CONSOLE))
		mainboard_machine_check();

	/* Configure requested TSEG size */
	switch (CONFIG_SMM_TSEG_SIZE) {
	case 1 * MiB:
		pci_update_config8(HOST_BRIDGE, ESMRAMC, ~TSEG_SZ_MASK, 0 << 1);
		break;
	case 2 * MiB:
		pci_update_config8(HOST_BRIDGE, ESMRAMC, ~TSEG_SZ_MASK, 1 << 1);
		break;
	case 8 * MiB:
		pci_update_config8(HOST_BRIDGE, ESMRAMC, ~TSEG_SZ_MASK, 2 << 1);
		break;
	default:
		printk(BIOS_WARNING, "%s: Unsupported TSEG size: 0x%x\n", __func__, CONFIG_SMM_TSEG_SIZE);
	}

	cbmem_recovery(0);
}
