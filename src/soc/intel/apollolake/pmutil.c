/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <console/console.h>
#include <rules.h>
#include <device/pci_def.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <device/device.h>
#include <device/pci.h>
#include <vendorcode/google/chromeos/vboot_common.h>

static uintptr_t read_pmc_mmio_bar(void)
{
	uint32_t bar = pci_read_config32(PMC_DEV, PCI_BASE_ADDRESS_0);
	return bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
}

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char * const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

static uint32_t print_smi_status(uint32_t smi_sts)
{
	static const char * const smi_sts_bits[] = {
		[2] = "BIOS",
		[3] = "LEGACY USB",
		[4] = "SLP_SMI",
		[5] = "APM",
		[6] = "SWSMI_TMR",
		[10]= "GPIO_SMI",
		[11]= "GPIO_UNLOCK_SSMI",
		[12] = "MCSMI",
		[13] = "TCO",
		[14] = "PERIODIC",
		[15] = "SERIRQ",
		[16] = "SMBUS_SMI",
		[17] = "XHCI",
		[18] = "HOST_SMBUS",
		[19] = "SCS",
		[20] = "PCI_EXP_SMI",
		[21] = "SCC2",
		[25] = "SPI_SSMI",
		[26] = "SPI",
		[27] = "OCP_CSE",
	};

	if (!smi_sts)
		return 0;

	printk(BIOS_DEBUG, "SMI_STS: ");
	print_num_status_bits(ARRAY_SIZE(smi_sts_bits), smi_sts, smi_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return smi_sts;
}

static uint32_t reset_smi_status(void)
{
	uint32_t smi_sts = inl(ACPI_PMIO_BASE + SMI_STS);
	outl(smi_sts, ACPI_PMIO_BASE + SMI_STS);
	return smi_sts;
}

uint32_t clear_smi_status(void)
{
	return print_smi_status(reset_smi_status());
}

uint32_t get_smi_en(void)
{
	return inl(ACPI_PMIO_BASE + SMI_EN);
}

void enable_smi(uint32_t mask)
{
	uint32_t smi_en = inl(ACPI_PMIO_BASE + SMI_EN);
	smi_en |= mask;
	outl(smi_en, ACPI_PMIO_BASE + SMI_EN);
}

void disable_smi(uint32_t mask)
{
	uint32_t smi_en = inl(ACPI_PMIO_BASE + SMI_EN);
	smi_en &= ~mask;
	outl(smi_en, ACPI_PMIO_BASE + SMI_EN);
}

void enable_pm1_control(uint32_t mask)
{
	uint32_t pm1_cnt = inl(ACPI_PMIO_BASE + PM1_CNT);
	pm1_cnt |= mask;
	outl(pm1_cnt, ACPI_PMIO_BASE + PM1_CNT);
}

void disable_pm1_control(uint32_t mask)
{
	uint32_t pm1_cnt = inl(ACPI_PMIO_BASE + PM1_CNT);
	pm1_cnt &= ~mask;
	outl(pm1_cnt, ACPI_PMIO_BASE + PM1_CNT);
}

static uint16_t reset_pm1_status(void)
{
	uint16_t pm1_sts = inw(ACPI_PMIO_BASE + PM1_STS);
	outw(pm1_sts, ACPI_PMIO_BASE + PM1_STS);
	return pm1_sts;
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char * const pm1_sts_bits[] = {
		[0] = "TMROF",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[11] = "PRBTNOR",
		[13] = "USB",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_SPEW, "PM1_STS: ");
	print_num_status_bits(ARRAY_SIZE(pm1_sts_bits), pm1_sts, pm1_sts_bits);
	printk(BIOS_SPEW, "\n");

	return pm1_sts;
}

uint16_t clear_pm1_status(void)
{
	return print_pm1_status(reset_pm1_status());
}

void enable_pm1(uint16_t events)
{
	outw(events, ACPI_PMIO_BASE + PM1_EN);
}

static uint32_t print_tco_status(uint32_t tco_sts)
{
	static const char * const tco_sts_bits[] = {
		[3] = "TIMEOUT",
		[17] = "SECOND_TO",
	};

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");
	print_num_status_bits(ARRAY_SIZE(tco_sts_bits), tco_sts, tco_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return tco_sts;
}

static uint32_t reset_tco_status(void)
{
	uint32_t tco_sts = inl(ACPI_PMIO_BASE + TCO_STS);
	uint32_t tco_en = inl(ACPI_PMIO_BASE + TCO1_CNT);

	outl(tco_sts, ACPI_PMIO_BASE + TCO_STS);
	return tco_sts & tco_en;
}

uint32_t clear_tco_status(void)
{
	return print_tco_status(reset_tco_status());
}

void enable_gpe(uint32_t mask)
{
	uint32_t gpe0a_en = inl(ACPI_PMIO_BASE + GPE0_EN(0));
	gpe0a_en |= mask;
	outl(gpe0a_en, ACPI_PMIO_BASE + GPE0_EN(0));
}

void disable_gpe(uint32_t mask)
{
	uint32_t gpe0a_en = inl(ACPI_PMIO_BASE + GPE0_EN(0));
	gpe0a_en &= ~mask;
	outl(gpe0a_en, ACPI_PMIO_BASE + GPE0_EN(0));
}

void disable_all_gpe(void)
{
	disable_gpe(~0);
}


static uint32_t reset_gpe_status(void)
{
	uint32_t gpe_sts = inl(ACPI_PMIO_BASE + GPE0_STS(0));
	outl(gpe_sts, ACPI_PMIO_BASE + GPE0_STS(0));
	return gpe_sts;
}

static uint32_t print_gpe_sts(uint32_t gpe_sts)
{
	static const char * const gpe_sts_bits[] = {
		[0] = "PCIE_SCI",
		[2] = "SWGPE",
		[3] = "PCIE_WAKE0",
		[4] = "PUNIT",
		[6] = "PCIE_WAKE1",
		[7] = "PCIE_WAKE2",
		[8] = "PCIE_WAKE3",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[11] = "CSE_PME",
		[12] = "XDCI_PME",
		[13] = "XHCI_PME",
		[14] = "AVS_PME",
		[15] = "GPIO_TIER1_SCI",
		[16] = "SMB_WAK",
		[17] = "SATA_PME",
	};

	if (!gpe_sts)
		return gpe_sts;

	printk(BIOS_DEBUG, "GPE0a_STS: ");
	print_num_status_bits(ARRAY_SIZE(gpe_sts_bits), gpe_sts, gpe_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return gpe_sts;
}

uint32_t clear_gpe_status(void)
{
	return print_gpe_sts(reset_gpe_status());
}

void clear_pmc_status(void)
{
	uint32_t prsts;
	uint32_t gen_pmcon1;
	uintptr_t pmc_bar0 = read_pmc_mmio_bar();

	prsts = read32((void *)(pmc_bar0 + PRSTS));
	gen_pmcon1 = read32((void *)(pmc_bar0 + GEN_PMCON1));

	/* Clear the status bits. The RPS field is cleared on a 0 write. */
	write32((void *)(pmc_bar0 + GEN_PMCON1), gen_pmcon1 & ~RPS);
	write32((void *)(pmc_bar0 + PRSTS), prsts);
}


/* Return 0, 3, or 5 to indicate the previous sleep state. */
int chipset_prev_sleep_state(struct chipset_power_state *ps)
{
	/* Default to S0. */
	int prev_sleep_state = SLEEP_STATE_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch ((ps->pm1_cnt & SLP_TYP) >> SLP_TYP_SHIFT) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		case SLP_TYP_S3:
			prev_sleep_state = SLEEP_STATE_S3;
			break;
#endif
		case SLP_TYP_S5:
			prev_sleep_state = SLEEP_STATE_S5;
			break;
		}
	}
	return prev_sleep_state;
}

/* returns prev_sleep_state */
int fill_power_state(struct chipset_power_state *ps)
{
	int i;
	uintptr_t pmc_bar0 = read_pmc_mmio_bar();

	ps->pm1_sts = inw(ACPI_PMIO_BASE + PM1_STS);
	ps->pm1_en = inw(ACPI_PMIO_BASE + PM1_EN);
	ps->pm1_cnt = inl(ACPI_PMIO_BASE + PM1_CNT);
	ps->tco_sts = inl(ACPI_PMIO_BASE + TCO_STS);
	ps->prsts = read32((void *)(pmc_bar0 + PRSTS));
	ps->gen_pmcon1 =read32((void *)(pmc_bar0 + GEN_PMCON1));
	ps->gen_pmcon2 = read32((void *)(pmc_bar0 + GEN_PMCON2));
	ps->gen_pmcon3 = read32((void *)(pmc_bar0 + GEN_PMCON3));

	ps->prev_sleep_state = chipset_prev_sleep_state(ps);

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
		ps->pm1_sts, ps->pm1_en, ps->pm1_cnt);
	printk(BIOS_DEBUG, "prsts: %08x tco_sts: %08x\n",
		ps->prsts, ps->tco_sts);
	printk(BIOS_DEBUG,
		 "gen_pmcon1: %08x gen_pmcon2: %08x gen_pmcon3: %08x\n",
		ps->gen_pmcon1, ps->gen_pmcon2, ps->gen_pmcon3);
	printk(BIOS_DEBUG, "smi_en: %08x smi_sts: %08x\n",
		inl(ACPI_PMIO_BASE + SMI_EN), inl(ACPI_PMIO_BASE + SMI_STS));
	for (i=0; i < GPE0_REG_MAX; i++) {
		ps->gpe0_sts[i] = inl(ACPI_PMIO_BASE + GPE0_STS(i));
		ps->gpe0_en[i] = inl(ACPI_PMIO_BASE + GPE0_EN(i));
		printk(BIOS_DEBUG, "gpe0_sts[%d]: %08x gpe0_en[%d]: %08x\n",
			i, ps->gpe0_sts[i], i, ps->gpe0_en[i]);
	}
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);
	return ps->prev_sleep_state;
}

int vboot_platform_is_resuming(void)
{
	int typ;

	if (!(inw(ACPI_PMIO_BASE + PM1_STS) & WAK_STS))
		return 0;

	typ = (inl(ACPI_PMIO_BASE + PM1_CNT) & SLP_TYP) >> SLP_TYP_SHIFT;
	return typ == SLP_TYP_S3;
}
