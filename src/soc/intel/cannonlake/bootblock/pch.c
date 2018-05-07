/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <device/device.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/gspi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <intelblocks/smbus.h>
#include <soc/bootblock.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>

#define PCR_PSF3_TO_SHDW_PMC_REG_BASE	0x1400
#define PCR_PSFX_TO_SHDW_BAR0	0
#define PCR_PSFX_TO_SHDW_BAR1	0x4
#define PCR_PSFX_TO_SHDW_BAR2	0x8
#define PCR_PSFX_TO_SHDW_BAR3	0xC
#define PCR_PSFX_TO_SHDW_BAR4	0x10
#define PCR_PSFX_TO_SHDW_PCIEN_IOEN	0x01
#define PCR_PSFX_T0_SHDW_PCIEN	0x1C

#define PCR_DMI_ACPIBA		0x27B4
#define PCR_DMI_ACPIBDID	0x27B8
#define PCR_DMI_PMBASEA		0x27AC
#define PCR_DMI_PMBASEC		0x27B0
#define PCR_DMI_TCOBASE		0x2778
#define  TCOEN			(1 << 1)	/* Enable TCO I/O range decode. */

#define PCR_DMI_LPCIOD		0x2770
#define PCR_DMI_LPCIOE		0x2774

static void soc_config_pwrmbase(void)
{
	uint32_t reg32;

	/* Assign Resources to PWRMBASE  */
	/* Clear BIT 1-2  Command Register */
	reg32 = pci_read_config32(PCH_DEV_PMC, PCI_COMMAND);
	reg32 &= ~(PCI_COMMAND_MEMORY);
	pci_write_config32(PCH_DEV_PMC, PCI_COMMAND, reg32);

	/* Program PWRM Base */
	pci_write_config32(PCH_DEV_PMC, PWRMBASE, PCH_PWRM_BASE_ADDRESS);

	/* Enable Bus Master and MMIO Space */
	reg32 = pci_read_config32(PCH_DEV_PMC, PCI_COMMAND);
	reg32 |=  PCI_COMMAND_MEMORY;
	pci_write_config32(PCH_DEV_PMC, PCI_COMMAND, reg32);

	/* Enable PWRM in PMC */
	reg32 = read32((void *)(PCH_PWRM_BASE_ADDRESS + ACTL));
	write32((void *)(PCH_PWRM_BASE_ADDRESS + ACTL), reg32 | PWRM_EN);
}

void bootblock_pch_early_init(void)
{
	fast_spi_early_init(SPI_BASE_ADDRESS);
	gspi_early_bar_init();
	p2sb_enable_bar();
	p2sb_configure_hpet();
	/*
	 * Enabling PWRM Base for accessing
	 * Global Reset Cause Register.
	 */
	soc_config_pwrmbase();
}


static void soc_config_acpibase(void)
{
	uint32_t pmc_reg_value;

	pmc_reg_value = pcr_read32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
						PCR_PSFX_TO_SHDW_BAR4);

	if (pmc_reg_value != 0xFFFFFFFF)
	{
		/* Disable Io Space before changing the address */
		pcr_rmw32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
				PCR_PSFX_T0_SHDW_PCIEN,
				~PCR_PSFX_TO_SHDW_PCIEN_IOEN, 0);
		/* Program ABASE in PSF3 PMC space BAR4*/
		pcr_write32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
				PCR_PSFX_TO_SHDW_BAR4,
				ACPI_BASE_ADDRESS);
		/* Enable IO Space */
		pcr_rmw32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
				PCR_PSFX_T0_SHDW_PCIEN,
				~0, PCR_PSFX_TO_SHDW_PCIEN_IOEN);
	}
}

static void soc_config_tco(void)
{
	uint32_t reg32;
	uint16_t tcobase;
	uint16_t tcocnt;

	/* Disable TCO in SMBUS Device first before changing Base Address */
	reg32 = pci_read_config32(PCH_DEV_SMBUS, TCOCTL);
	reg32 &= ~TCO_BASE_EN;
	pci_write_config32(PCH_DEV_SMBUS, TCOCTL, reg32);

	/* Program TCO Base */
	tcobase = TCO_BASE_ADDRESS;
	pci_write_config32(PCH_DEV_SMBUS, TCOBASE, tcobase);

	/* Enable TCO in SMBUS */
	pci_write_config32(PCH_DEV_SMBUS, TCOCTL, reg32 | TCO_BASE_EN);

	/*
	 * Program "TCO Base Address" PCR[DMI] + 2778h[15:5, 1]
	 */
	pcr_write32(PID_DMI, PCR_DMI_TCOBASE, tcobase | TCOEN);

	/* Program TCO timer halt */
	tcocnt = inw(tcobase + TCO1_CNT);
	tcocnt |= TCO_TMR_HLT;
	outw(tcocnt, tcobase + TCO1_CNT);
}

void pch_early_iorange_init(void)
{
	uint16_t dec_rng, dec_en = 0;

	/* IO Decode Range */
	if (IS_ENABLED(CONFIG_DRIVERS_UART_8250IO) &&
	    IS_ENABLED(CONFIG_UART_DEBUG)) {
		dec_rng = COMA_RANGE | (COMB_RANGE << 4);
		dec_en = COMA_LPC_EN | COMB_LPC_EN;
		pci_write_config16(PCH_DEV_LPC, LPC_IO_DEC, dec_rng);
		pcr_write16(PID_DMI, PCR_DMI_LPCIOD, dec_rng);
	}

	/* IO Decode Enable */
	dec_en |= SE_LPC_EN | KBC_LPC_EN | MC1_LPC_EN | GAMEL_LPC_EN;
	pci_write_config16(PCH_DEV_LPC, LPC_EN, dec_en);
	pcr_write16(PID_DMI, PCR_DMI_LPCIOE, dec_en);

	/* Program generic IO Decode Range */
	pch_enable_lpc();
}

void pch_early_init(void)
{
	/*
	 * Enabling ABASE for accessing PM1_STS, PM1_EN, PM1_CNT,
	 * GPE0_STS, GPE0_EN registers.
	 */
	soc_config_acpibase();

	/* Programming TCO_BASE_ADDRESS and TCO Timer Halt */
	soc_config_tco();

	/* Program SMBUS_BASE_ADDRESS and Enable it */
	smbus_common_init();

	/* Set up GPE configuration */
	pmc_gpe_init();

	enable_rtc_upper_bank();
}
