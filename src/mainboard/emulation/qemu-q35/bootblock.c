/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <bootblock_common.h>
#include <southbridge/intel/common/early_spi.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <console/console.h>

#include "q35.h"

static void bootblock_northbridge_init(void)
{
	uint32_t reg;

	/*
	 * The "io" variant of the config access is explicitly used to
	 * setup the PCIEXBAR because CONFIG(MMCONF_SUPPORT) is set to
	 * to true. That way all subsequent non-explicit config accesses use
	 * MCFG. This code also assumes that bootblock_northbridge_init() is
	 * the first thing called in the non-asm boot block code. The final
	 * assumption is that no assembly code is using the
	 * CONFIG(MMCONF_SUPPORT) option to do PCI config acceses.
	 *
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */
	reg = 0;
	pci_io_write_config32(HOST_BRIDGE, D0F0_PCIEXBAR_HI, reg);
	reg = CONFIG_MMCONF_BASE_ADDRESS | 1; /* 256MiB - 0-255 buses. */
	pci_io_write_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO, reg);

	/* MCFG is now active. If it's not qemu was started for machine PC */
	if (CONFIG(BOOTBLOCK_CONSOLE) &&
	    (pci_read_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO) !=
	     (CONFIG_MMCONF_BASE_ADDRESS | 1)))
		die("You must run qemu for machine Q35 (-M q35)");
}

static void bootblock_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	/* Enable RCBA */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), RCBA,
			(uintptr_t)DEFAULT_RCBA | 1);
}

void bootblock_soc_init(void)
{
	bootblock_northbridge_init();
	bootblock_southbridge_init();
}
