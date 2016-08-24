/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <soc/pci_devs.h>
#include <soc/pm.h>

void pmc_set_disb(void)
{
	/* Set the DISB after DRAM init */
	u32 disb_val = 0;
	pci_devfn_t dev = PCH_DEV_PMC;
	disb_val = pci_read_config32(dev, GEN_PMCON_A);
	disb_val |= DISB;

	/* Don't clear bits that are write-1-to-clear */
	disb_val &= ~(GBL_RST_STS | MS4V);
	pci_write_config32(dev, GEN_PMCON_A, disb_val);
}
