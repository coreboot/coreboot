/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stddef.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/romstage.h>
#include "../chip.h"

void tco_disable(void)
{
	uint32_t reg;

	reg = inl(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg |= TCO_TMR_HALT;
	outl(reg, ACPI_BASE_ADDRESS + TCO1_CNT);
}

/* This sequence signals the PUNIT to start running. */
void punit_init(void)
{
	uint32_t reg;
	uint8_t rid;
	const struct device *dev;
	const struct soc_intel_baytrail_config *cfg = NULL;

	rid = pci_read_config8(IOSF_PCI_DEV, REVID);
	dev = dev_find_slot(0, PCI_DEVFN(SOC_DEV, SOC_FUNC));

	if (dev)
		cfg = dev->chip_info;

	reg = iosf_punit_read(SB_BIOS_CONFIG);
	/* Write bits 17:16 of SB_BIOS_CONFIG in the PUNIT. */
	reg |= SB_BIOS_CONFIG_PERF_MODE | SB_BIOS_CONFIG_PDM_MODE;
	/* Configure VR low power mode for C0 and above. */
	if (rid >= RID_C_STEPPING_START && cfg != NULL &&
	    (cfg->vnn_ps2_enable || cfg->vcc_ps2_enable)) {
		printk(BIOS_DEBUG, "Enabling VR PS2 mode:");
		if (cfg->vnn_ps2_enable) {
			reg |= SB_BIOS_CONFIG_PS2_EN_VNN;
			printk(BIOS_DEBUG, " VNN");
		}
		if (cfg->vcc_ps2_enable) {
			reg |= SB_BIOS_CONFIG_PS2_EN_VCC;
			printk(BIOS_DEBUG, " VCC");
		}
		printk(BIOS_DEBUG, "\n");
	}
	iosf_punit_write(SB_BIOS_CONFIG, reg);

	/* Write bits 1:0 of BIOS_RESET_CPL in the PUNIT. */
	reg = BIOS_RESET_CPL_ALL_DONE | BIOS_RESET_CPL_RESET_DONE;
	pci_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_PMC) | IOSF_PORT(IOSF_PORT_PMC) |
	     IOSF_REG(BIOS_RESET_CPL) | IOSF_BYTE_EN_0;
	pci_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}
