/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
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

#include <stdlib.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <reg_script.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>

static const struct reg_script systemagent_early_init_script[] = {
	REG_PCI_WRITE32(MCHBAR, MCH_BASE_ADDRESS | 1),
	REG_PCI_WRITE32(DMIBAR, DMI_BASE_ADDRESS | 1),
	REG_PCI_WRITE32(EPBAR, EP_BASE_ADDRESS | 1),
	REG_MMIO_WRITE32(MCH_BASE_ADDRESS + EDRAMBAR, EDRAM_BASE_ADDRESS | 1),
	REG_MMIO_WRITE32(MCH_BASE_ADDRESS + GDXCBAR, GDXC_BASE_ADDRESS | 1),

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	REG_PCI_WRITE8(PAM0, 0x30),
	REG_PCI_WRITE8(PAM1, 0x33),
	REG_PCI_WRITE8(PAM2, 0x33),
	REG_PCI_WRITE8(PAM3, 0x33),
	REG_PCI_WRITE8(PAM4, 0x33),
	REG_PCI_WRITE8(PAM5, 0x33),
	REG_PCI_WRITE8(PAM6, 0x33),

	/* Device enable: IGD and Mini-HD */
	REG_PCI_WRITE32(DEVEN, DEVEN_D0EN | DEVEN_D2EN | DEVEN_D3EN),

	REG_SCRIPT_END
};

void systemagent_early_init(void)
{
	reg_script_run_on_dev(SA_DEV_ROOT, systemagent_early_init_script);
}
