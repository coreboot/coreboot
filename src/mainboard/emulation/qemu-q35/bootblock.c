/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <bootblock_common.h>
#include <southbridge/intel/common/early_spi.h>
#include <southbridge/intel/i82801ix/i82801ix.h>

#include "q35.h"

static void bootblock_northbridge_init(void)
{
	/*
	 * The "io" variant of the config access is explicitly used to
	 * setup the PCIEXBAR because CONFIG(ECAM_MMCONF_SUPPORT) is set
	 * to true. That way all subsequent non-explicit config accesses use
	 * MCFG. This code also assumes that bootblock_northbridge_init() is
	 * the first thing called in the non-asm boot block code. The final
	 * assumption is that no assembly code is using the
	 * CONFIG(ECAM_MMCONF_SUPPORT) option to do PCI config accesses.
	 *
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */
	const uint32_t pciexbar = make_pciexbar();
	pci_io_write_config32(HOST_BRIDGE, D0F0_PCIEXBAR_HI, 0);
	pci_io_write_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO, pciexbar);

	if (CONFIG(BOOTBLOCK_CONSOLE))
		mainboard_machine_check();
}

static void bootblock_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	/* Enable RCBA */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), RCBA,
			CONFIG_FIXED_RCBA_MMIO_BASE | 1);
}

void bootblock_soc_init(void)
{
	bootblock_northbridge_init();
	bootblock_southbridge_init();
}
