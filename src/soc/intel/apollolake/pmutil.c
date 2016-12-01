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

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/msr.h>
#include <rules.h>
#include <device/pci_def.h>
#include <halt.h>
#include <soc/iomap.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <device/device.h>
#include <device/pci.h>
#include <timer.h>
#include <vboot/vboot_common.h>
#include "chip.h"

static uintptr_t read_pmc_mmio_bar(void)
{
	return PMC_BAR0;
}

uintptr_t get_pmc_mmio_bar(void)
{
	return read_pmc_mmio_bar();
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
		[BIOS_SMI_STS] = "BIOS",
		[LEGACY_USB_SMI_STS] = "LEGACY USB",
		[SLP_SMI_STS] = "SLP_SMI",
		[APM_SMI_STS] = "APM",
		[SWSMI_TMR_SMI_STS] = "SWSMI_TMR",
		[FAKE_PM1_SMI_STS] = "PM1",
		[GPIO_SMI_STS]= "GPIO_SMI",
		[GPIO_UNLOCK_SMI_STS]= "GPIO_UNLOCK_SSMI",
		[MC_SMI_STS] = "MCSMI",
		[TCO_SMI_STS] = "TCO",
		[PERIODIC_SMI_STS] = "PERIODIC",
		[SERIRQ_SMI_STS] = "SERIRQ",
		[SMBUS_SMI_STS] = "SMBUS_SMI",
		[XHCI_SMI_STS] = "XHCI",
		[HSMBUS_SMI_STS] = "HOST_SMBUS",
		[SCS_SMI_STS] = "SCS",
		[PCIE_SMI_STS] = "PCI_EXP_SMI",
		[SCC2_SMI_STS] = "SCC2",
		[SPI_SSMI_STS] = "SPI_SSMI",
		[SPI_SMI_STS] = "SPI",
		[PMC_OCP_SMI_STS] = "OCP_CSE",
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
	uint32_t sts = reset_smi_status();

	/*
	 * Check for power button status if nothing else is indicating an SMI
	 * and SMIs aren't turned into SCIs. Apparently, there is no PM1 status
	 * bit in the SMI status register.  That makes things difficult for
	 * determining if the power button caused an SMI.
	 */
	if (sts == 0 && !(inl(ACPI_PMIO_BASE + PM1_CNT) & SCI_EN)) {
		uint16_t pm1_sts = inw(ACPI_PMIO_BASE + PM1_STS);

		/* Fake PM1 status bit if power button pressed. */
		if (pm1_sts & PWRBTN_STS)
			sts |= (1 << FAKE_PM1_SMI_STS);
	}

	return print_smi_status(sts);
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

/* Clear the gpio gpe0 status bits in ACPI registers */
void clear_gpi_gpe_sts(void)
{
	int i;

	for (i = 1; i < GPE0_REG_MAX; i++) {
		uint32_t gpe_sts = inl(ACPI_PMIO_BASE + GPE0_STS(i));
		outl(gpe_sts, ACPI_PMIO_BASE + GPE0_STS(i));
	}
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

/* Read and clear GPE status (defined in arch/acpi.h) */
int acpi_get_gpe(int gpe)
{
	int bank;
	uint32_t mask, sts;
	struct stopwatch sw;
	int rc = 0;

	if (gpe < 0 || gpe > GPE0_DW3_31)
		return -1;

	bank = gpe / 32;
	mask = 1 << (gpe % 32);

	/* Wait up to 1ms for GPE status to clear */
	stopwatch_init_msecs_expire(&sw, 1);
	do {
		if (stopwatch_expired(&sw))
			return rc;

		sts = inl(ACPI_PMIO_BASE + GPE0_STS(bank));
		if (sts & mask) {
			outl(mask, ACPI_PMIO_BASE + GPE0_STS(bank));
			rc = 1;
		}
	} while (sts & mask);

	return rc;
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
	int prev_sleep_state = ACPI_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch (acpi_sleep_from_pm1(ps->pm1_cnt)) {
		case ACPI_S3:
			if (IS_ENABLED(CONFIG_HAVE_ACPI_RESUME))
				prev_sleep_state = ACPI_S3;
			break;
		case ACPI_S5:
			prev_sleep_state = ACPI_S5;
			break;
		}

		/* Clear SLP_TYP. */
		outl(ps->pm1_cnt & ~(SLP_TYP), ACPI_PMIO_BASE + PM1_CNT);
	}
	return prev_sleep_state;
}

/*
 * This function re-writes the gpe0 register values in power state
 * cbmem variable. After system wakes from sleep state internal PMC logic
 * writes default values in GPE_CFG register which gives a wrong offset to
 * calculate the wake reason. So we need to set it again to the routing
 * table as per the devicetree.
 */
void fixup_power_state(void)
{
	int i;
	struct chipset_power_state *ps;

	ps = cbmem_find(CBMEM_ID_POWER_STATE);
	if (ps == NULL)
		return;

	for (i = 0; i < GPE0_REG_MAX; i++) {
		ps->gpe0_sts[i] = inl(ACPI_PMIO_BASE + GPE0_STS(i));
		ps->gpe0_en[i] = inl(ACPI_PMIO_BASE + GPE0_EN(i));
		printk(BIOS_DEBUG, "gpe0_sts[%d]: %08x gpe0_en[%d]: %08x\n",
					i, ps->gpe0_sts[i], i, ps->gpe0_en[i]);
	}
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
	if (!(inw(ACPI_PMIO_BASE + PM1_STS) & WAK_STS))
		return 0;

	return acpi_sleep_from_pm1(inl(ACPI_PMIO_BASE + PM1_CNT)) == ACPI_S3;
}

/*
 * If possible, lock 0xcf9. Once the register is locked, it can't be changed.
 * This lock is reset on cold boot, hard reset, soft reset and Sx.
 */
void global_reset_lock(void)
{
	uintptr_t etr = read_pmc_mmio_bar() + ETR;
	uint32_t reg;

	reg = read32((void *)etr);
	if (reg & CF9_LOCK)
		return;
	reg |= CF9_LOCK;
	write32((void *)etr, reg);
}

/*
 * Enable or disable global reset. If global reset is enabled, hard reset and
 * soft reset will trigger global reset, where both host and TXE are reset.
 * This is cleared on cold boot, hard reset, soft reset and Sx.
 */
void global_reset_enable(bool enable)
{
	uintptr_t etr = read_pmc_mmio_bar() + ETR;
	uint32_t reg;

	reg = read32((void *)etr);
	reg = enable ? reg | CF9_GLB_RST : reg & ~CF9_GLB_RST;
	write32((void *)etr, reg);
}

/*
 * The PM1 control is set to S5 when vboot requests a reboot because the power
 * state code above may not have collected its data yet. Therefore, set it to
 * S5 when vboot requests a reboot. That's necessary if vboot fails in the
 * resume path and requests a reboot. This prevents a reboot loop where the
 * error is continually hit on the failing vboot resume path.
 */
void vboot_platform_prepare_reboot(void)
{
	const uint16_t port = ACPI_PMIO_BASE + PM1_CNT;
	outl((inl(port) & ~(SLP_TYP)) | (SLP_TYP_S5 << SLP_TYP_SHIFT), port);
}

void poweroff(void)
{
	enable_pm1_control(SLP_EN | (SLP_TYP_S5 << SLP_TYP_SHIFT));

	/*
	 * Setting SLP_TYP_S5 in PM1 triggers SLP_SMI, which is handled by SMM
	 * to transition to S5 state. If halt is called in SMM, then it prevents
	 * the SMI handler from being triggered and system never enters S5.
	 */
	if (!ENV_SMM)
		halt();
}

void pmc_gpe_init(void)
{
	uint32_t gpio_cfg = 0;
	uint32_t gpio_cfg_reg;
	uint8_t dw1, dw2, dw3;
	ROMSTAGE_CONST struct soc_intel_apollolake_config *config;

	/* Look up the device in devicetree */
	ROMSTAGE_CONST struct device *dev = dev_find_slot(0, NB_DEVFN);
	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}
	config = dev->chip_info;

	uintptr_t pmc_bar = get_pmc_mmio_bar();

	const uint32_t gpio_cfg_mask =
		(GPE0_DWX_MASK << GPE0_DW1_SHIFT) |
		(GPE0_DWX_MASK << GPE0_DW2_SHIFT) |
		(GPE0_DWX_MASK << GPE0_DW3_SHIFT);

	/* Assign to local variable */
	dw1 = config->gpe0_dw1;
	dw2 = config->gpe0_dw2;
	dw3 = config->gpe0_dw3;

	/* Making sure that bad values don't bleed into the other fields */
	dw1 &= GPE0_DWX_MASK;
	dw2 &= GPE0_DWX_MASK;
	dw3 &= GPE0_DWX_MASK;

	/* Route the GPIOs to the GPE0 block. Determine that all values
	 * are different, and if they aren't use the reset values.
	 * DW0 is reserved/unused */
	if (dw1 == dw2 || dw2 == dw3) {
		printk(BIOS_INFO, "PMC: Using default GPE route.\n");
		gpio_cfg = read32((void *)pmc_bar + GPIO_GPE_CFG);

		dw1 = (gpio_cfg >> GPE0_DW1_SHIFT) & GPE0_DWX_MASK;
		dw2 = (gpio_cfg >> GPE0_DW2_SHIFT) & GPE0_DWX_MASK;
		dw3 = (gpio_cfg >> GPE0_DW3_SHIFT) & GPE0_DWX_MASK;
	} else {
		gpio_cfg |= (uint32_t)dw1 << GPE0_DW1_SHIFT;
		gpio_cfg |= (uint32_t)dw2 << GPE0_DW2_SHIFT;
		gpio_cfg |= (uint32_t)dw3 << GPE0_DW3_SHIFT;
	}

	gpio_cfg_reg = read32((void *)pmc_bar + GPIO_GPE_CFG) & ~gpio_cfg_mask;
	gpio_cfg_reg |= gpio_cfg & gpio_cfg_mask;

	write32((void *)pmc_bar + GPIO_GPE_CFG, gpio_cfg_reg);

	/* Set the routes in the GPIO communities as well. */
	gpio_route_gpe(dw1, dw2, dw3);
}

void enable_pm_timer_emulation(void)
{
	/* ACPI PM timer emulation */
	msr_t msr;
	/*
	 * The derived frequency is calculated as follows:
	 *    (CTC_FREQ * msr[63:32]) >> 32 = target frequency.
	 * Back solve the multiplier so the 3.579545MHz ACPI timer
	 * frequency is used.
	 */
	msr.hi = (3579545ULL << 32) / CTC_FREQ;
	/* Set PM1 timer IO port and enable*/
	msr.lo = EMULATE_PM_TMR_EN | (ACPI_PMIO_BASE + R_ACPI_PM1_TMR);
	wrmsr(MSR_EMULATE_PM_TMR, msr);
}
