/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <commonlib/helpers.h>
#include <intelblocks/systemagent.h>
#include <soc/pci_devs.h>

void bootblock_systemagent_early_init(void)
{
	uint32_t reg;
	uint8_t pciexbar_length;

	/*
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */
	reg = 0;
	pci_io_write_config32(SA_DEV_ROOT, PCIEXBAR + 4, reg);

	/* Get PCI Express Region Length */
	switch (CONFIG_SA_PCIEX_LENGTH) {
	case 256 * MiB:
		pciexbar_length = PCIEXBAR_LENGTH_256MB;
		break;
	case 128 * MiB:
		pciexbar_length = PCIEXBAR_LENGTH_128MB;
		break;
	case 64 * MiB:
		pciexbar_length = PCIEXBAR_LENGTH_64MB;
		break;
	default:
		pciexbar_length = PCIEXBAR_LENGTH_256MB;
	}
	reg = CONFIG_MMCONF_BASE_ADDRESS | (pciexbar_length << 1)
				| PCIEXBAR_PCIEXBAREN;
	pci_io_write_config32(SA_DEV_ROOT, PCIEXBAR, reg);

	/*
	 * TSEG defines the base of SMM range. BIOS determines the base
	 * of TSEG memory which must be at or below Graphics base of GTT
	 * Stolen memory, hence its better to clear TSEG register early
	 * to avoid power on default non-zero value (if any).
	 */
	pci_write_config32(SA_DEV_ROOT, TSEG, 0);
}

