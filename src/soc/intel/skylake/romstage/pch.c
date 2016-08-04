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
	lpc_en = COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN;
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

static void soc_config_acpibase(void)
{
	uint32_t reg32;

	/* Disable ABASE in PMC Device first before changing Base Address*/
	reg32 = pci_read_config32(PCH_DEV_PMC, ACTL);
	pci_write_config32(PCH_DEV_PMC, ACTL, reg32 & ~ACPI_EN);

	/* Program ACPI Base */
	pci_write_config32(PCH_DEV_PMC, ABASE, ACPI_BASE_ADDRESS);

	/* Enable ACPI in PMC */
	pci_write_config32(PCH_DEV_PMC, ACTL, reg32 | ACPI_EN);

	/*
	 * Program "ACPI Base Address" PCR[DMI] + 27B4h[23:18, 15:2, 0]
	 * to [0x3F, PMC PCI Offset 40h bit[15:2], 1]
	 */
	reg32 = ((0x3f << 18) | ACPI_BASE_ADDRESS | 1);
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_ACPIBA, reg32);
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_ACPIBDID, 0x23A0);
}

static void soc_config_pwrmbase(void)
{
	uint32_t reg32;

	/* Disable PWRMBASE in PMC Device first before changing Base address */
	reg32 = pci_read_config32(PCH_DEV_PMC, ACTL);
	pci_write_config32(PCH_DEV_PMC, ACTL, reg32 & ~PWRM_EN);

	/* Program PWRM Base */
	pci_write_config32(PCH_DEV_PMC, PWRMBASE, PCH_PWRM_BASE_ADDRESS);

	/* Enable PWRM in PMC */
	pci_write_config32(PCH_DEV_PMC, ACTL, reg32 | PWRM_EN);

	/*
	 * Program "PM Base Address Memory Range Base" PCR[DMI] + 27ACh[15:0]
	 * to the same value programmed in PMC PCI Offset 48h bit[31:16],
	 * this has an implication of making sure the PWRMBASE to be
	 * 64KB aligned.
	 *
	 * Program "PM Base Address Memory Range Limit" PCR[DMI] + 27ACh[31:16]
	 * to the value programmed in PMC PCI Offset 48h bit[31:16], this has an
	 * implication of making sure the memory allocated to PWRMBASE to be 64KB
	 * in size.
	 */
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_PMBASEA,
		((PCH_PWRM_BASE_ADDRESS & 0xFFFF0000) |
		 (PCH_PWRM_BASE_ADDRESS >> 16)));
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_PMBASEC, 0x800023A0);
}

static void soc_config_tco(void)
{
	uint32_t reg32 = 0;
	uint16_t tcobase;
	uint16_t tcocnt;

	/* Disable TCO in SMBUS Device first before changing Base Address */
	reg32 = pci_read_config32(PCH_DEV_SMBUS, TCOCTL);
	reg32 &= ~SMBUS_TCO_EN;
	pci_write_config32(PCH_DEV_SMBUS, TCOCTL, reg32);

	/* Program TCO Base */
	pci_write_config32(PCH_DEV_SMBUS, TCOBASE, TCO_BASE_ADDDRESS);

	/* Enable TCO in SMBUS */
	pci_write_config32(PCH_DEV_SMBUS, TCOCTL, reg32 | SMBUS_TCO_EN);

	/*
	 * Program "TCO Base Address" PCR[DMI] + 2778h[15:5, 1]
	 * to [SMBUS PCI offset 50h[15:5], 1].
	 */
	pcr_write32(PID_DMI, R_PCH_PCR_DMI_TCOBASE,
		   (TCO_BASE_ADDDRESS | (1 << 1)));

	/* Program TCO timer halt */
	tcobase = pci_read_config16(PCH_DEV_SMBUS, TCOBASE);
	tcobase &= ~0x1f;
	tcocnt = inw(tcobase + TCO1_CNT);
	tcocnt |= TCO_TMR_HLT;
	outw(tcocnt, tcobase + TCO1_CNT);
}

static void soc_config_rtc(void)
{
	/* Enable upper 128 bytes of CMOS */
	pcr_andthenor32(PID_RTC, R_PCH_PCR_RTC_CONF, ~0,
			B_PCH_PCR_RTC_CONF_UCMOS_EN);
}

void pch_early_init(void)
{
	/*
	 * Enabling ABASE for accessing PM1_STS, PM1_EN, PM1_CNT,
	 * GPE0_STS, GPE0_EN registers.
	 */
	soc_config_acpibase();

	/*
	 * Enabling PWRM Base for accessing
	 * Global Reset Cause Register.
	 */
	soc_config_pwrmbase();

	/* Programming TCO_BASE_ADDRESS and TCO Timer Halt */
	soc_config_tco();

	/*
	 * Interrupt Configuration Register Programming
	 * PIRQx to IRQ Programming
	 */
	pch_interrupt_init();

	/* Program generic IO Decode Range */
	pch_enable_lpc();

	/* Program SMBUS_BASE_ADDRESS and Enable it */
	enable_smbus();

	soc_config_rtc();
}
