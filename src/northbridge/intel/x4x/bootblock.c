/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/bootblock.h>
#include <arch/mmio.h>
#include <assert.h>
#include <device/pci_ops.h>
#include <types.h>

#include "x4x.h"

static uint32_t encode_pciexbar_length(void)
{
	switch (CONFIG_ECAM_MMCONF_BUS_NUMBER) {
		case 256: return 0 << 1;
		case 128: return 1 << 1;
		case  64: return 2 << 1;
		default:  return dead_code_t(uint32_t);
	}
}

void bootblock_early_northbridge_init(void)
{
	/* Disable LaGrande Technology (LT) */
	read32p(TPM_BASE_ADDRESS);

	const uint32_t reg32 = CONFIG_ECAM_MMCONF_BASE_ADDRESS | encode_pciexbar_length() | 1;
	pci_io_write_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO, reg32);
}
