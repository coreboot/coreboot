/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <reg_script.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/romstage.h>
#include <soc/smbus.h>

/* Max PXRC registers in ITSS*/
#define MAX_PXRC_CONFIG		0x08

static const u8 pch_interrupt_routing[] = {
		11,	/* PARC: PIRQA -> IRQ11 */
		10,	/* PBRC: PIRQB -> IRQ10 */
		11,	/* PCRC: PIRQC -> IRQ11 */
		11,	/* PDRC: PIRQD -> IRQ11 */
		11,	/* PERC: PIRQE -> IRQ11 */
		11,	/* PFRC: PIRQF -> IRQ11 */
		11,	/* PGRC: PIRQG -> IRQ11 */
		11	/* PHRC: PIRQH -> IRQ11 */
};

static void pch_enable_lpc(void)
{
	/* Lookup device tree in romstage */
	const struct device *dev;
	const config_t *config;
	u16 lpc_en;

	/* IO Decode Range */
	lpc_en = COMA_RANGE | (COMB_RANGE << 4);
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DEC, lpc_en);
	pcr_write16(PID_DMI, R_PCH_PCR_DMI_LPCIOD, lpc_en);

	/* IO Decode Enable */
	lpc_en = CNF1_LPC_EN | CNF2_LPC_EN | GAMEL_LPC_EN | GAMEH_LPC_EN |
		COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN;
	pci_write_config16(PCH_DEV_LPC, LPC_EN, lpc_en);
	pcr_write16(PID_DMI, R_PCH_PCR_DMI_LPCIOE, lpc_en);

	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	if (!dev || !dev->chip_info)
		return;
	config = dev->chip_info;

	/* Set in PCI generic decode range registers */
	pci_write_config32(PCH_DEV_LPC, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_DEV_LPC, LPC_GEN4_DEC, config->gen4_dec);

	/* Mirror these same settings in DMI PCR */
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_LPCLGIR1, config->gen1_dec);
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_LPCLGIR2, config->gen2_dec);
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_LPCLGIR3, config->gen3_dec);
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_LPCLGIR4, config->gen4_dec);
}

static void pch_device_init(void)
{
	device_t dev;
	u32 reg32;
	u16 tcobase;
	u16 tcocnt;

	dev = PCH_DEV_PMC;

	/* Enable ACPI and PMC mmio regs in PMC Config */
	reg32 = pci_read_config32(dev, ACTL);
	reg32 |= ACPI_EN | PWRM_EN;
	pci_write_config32(dev, ACTL, reg32);

	/* TCO timer halt */
	tcobase = pmc_tco_regs();
	tcocnt = inw(tcobase + TCO1_CNT);
	tcocnt |= TCO_TMR_HLT;
	outw(tcocnt, tcobase + TCO1_CNT);

	/* Enable upper 128 bytes of CMOS */
	pcr_andthenor32(PID_RTC, R_PCH_PCR_RTC_CONF, (u32)~0,
			B_PCH_PCR_RTC_CONF_UCMOS_EN);
}

static void pch_interrupt_init(void)
{
	u8 index = 0;

	for (index = 0; index < MAX_PXRC_CONFIG; index++) {
		if (pch_interrupt_routing[index] < 16 &&
			pch_interrupt_routing[index] > 2 &&
			pch_interrupt_routing[index] != 8 &&
			pch_interrupt_routing[index] != 13) {
				pcr_write8(PID_ITSS,
					(R_PCH_PCR_ITSS_PIRQA_ROUT + index),
					pch_interrupt_routing[index]);
		}
	}
}

void pch_early_init(void)
{
	pch_device_init();

	pch_interrupt_init();

	pch_enable_lpc();

	enable_smbus();
}
