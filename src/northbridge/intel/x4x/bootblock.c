/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/bootblock.h>
#include <arch/mmio.h>
#include <device/pci_ops.h>

#include "x4x.h"
#include "iomap.h"

void bootblock_early_northbridge_init(void)
{
	/* Disable LaGrande Technology (LT) */
	read32((void *)TPM_BASE_ADDRESS);

	const uint32_t reg32 = CONFIG_MMCONF_BASE_ADDRESS | 16 | 1;
	pci_io_write_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO, reg32);
}
