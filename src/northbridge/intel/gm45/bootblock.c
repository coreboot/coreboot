/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>

#include "gm45.h"

void bootblock_early_northbridge_init(void)
{
	uint32_t reg;

	/*
	 * The "io" variant of the config access is explicitly used to
	 * setup the PCIEXBAR because CONFIG(MMCONF_SUPPORT) is set to
	 * true. That way all subsequent non-explicit config accesses use
	 * MCFG. This code also assumes that bootblock_northbridge_init() is
	 * the first thing called in the non-asm boot block code. The final
	 * assumption is that no assembly code is using the
	 * CONFIG(MMCONF_SUPPORT) option to do PCI config accesses.
	 *
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */
	reg = 0;
	pci_io_write_config32(PCI_DEV(0,0,0), D0F0_PCIEXBAR_HI, reg);
	reg = CONFIG_MMCONF_BASE_ADDRESS | (2 << 1) | 1; /* 64MiB - 0-63 buses. */
	pci_io_write_config32(PCI_DEV(0,0,0), D0F0_PCIEXBAR_LO, reg);
}
