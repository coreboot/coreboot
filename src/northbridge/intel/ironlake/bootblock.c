/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <arch/pci_io_cfg.h>
#include <assert.h>
#include <types.h>

#include "ironlake.h"

static uint32_t encode_pciexbar_length(void)
{
	/* NOTE: Ironlake uses a different encoding for the PCIEXBAR length field */
	switch (CONFIG_MMCONF_BUS_NUMBER) {
		case 256: return 0 << 1;
		case 128: return 6 << 1;
		case  64: return 7 << 1;
		default:  return dead_code_t(uint32_t);
	}
}

void bootblock_early_northbridge_init(void)
{
	/*
	 * The QuickPath bus number is the topmost bus number, as per the value
	 * of the SAD_PCIEXBAR register. The register defaults to 256 busses on
	 * reset. Thus, hardcode the bus number when first setting up PCIEXBAR.
	 */
	const pci_devfn_t qpi_sad = PCI_DEV(255, 0, 1);

	const uint32_t reg32 = CONFIG_MMCONF_BASE_ADDRESS | encode_pciexbar_length() | 1;
	pci_io_write_config32(qpi_sad, SAD_PCIEXBAR + 4, 0);
	pci_io_write_config32(qpi_sad, SAD_PCIEXBAR, reg32);
}
