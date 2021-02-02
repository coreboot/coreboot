/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <assert.h>
#include <device/pci_ops.h>
#include <types.h>

#include "sandybridge.h"

static uint32_t encode_pciexbar_length(void)
{
	switch (CONFIG_MMCONF_BUS_NUMBER) {
		case 256: return 0 << 1;
		case 128: return 1 << 1;
		case  64: return 2 << 1;
		default:  return dead_code_t(uint32_t);
	}
}

void bootblock_early_northbridge_init(void)
{
	/*
	 * The "io" variant of the config access is explicitly used to setup the
	 * PCIEXBAR because CONFIG(MMCONF_SUPPORT) is set to true. That way, all
	 * subsequent non-explicit config accesses use MCFG. This code also assumes
	 * that bootblock_northbridge_init() is the first thing called in the non-asm
	 * boot block code. The final assumption is that no assembly code is using the
	 * CONFIG(MMCONF_SUPPORT) option to do PCI config accesses.
	 *
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under 4GiB.
	 */
	const uint32_t reg32 = CONFIG_MMCONF_BASE_ADDRESS | encode_pciexbar_length() | 1;
	pci_io_write_config32(HOST_BRIDGE, PCIEXBAR + 4, 0);
	pci_io_write_config32(HOST_BRIDGE, PCIEXBAR, reg32);
}
