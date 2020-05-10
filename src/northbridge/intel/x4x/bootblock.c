/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "x4x.h"
#include "iomap.h"

void bootblock_early_northbridge_init(void)
{
	uint32_t reg32;

	/* Disable LaGrande Technology (LT) */
	reg32 = TPM32(0);

	reg32 = CONFIG_MMCONF_BASE_ADDRESS | 16 | 1;
	pci_io_write_config32(PCI_DEV(0, 0, 0), D0F0_PCIEXBAR_LO, reg32);
}
