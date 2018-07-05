/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <cbmem.h>
#include <stddef.h>
#include <arch/early_variables.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/cbfs.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <chip.h>
#include <cpu/x86/mtrr.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <elog.h>
#include <mrc_cache.h>
#include <romstage_handoff.h>
#include <string.h>
#include <timestamp.h>
#include <reset.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <fsp/util.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/smm.h>
#include <soc/spi.h>
#include <security/tpm/tspi.h>

void program_base_addresses(void)
{
	uint32_t reg;
	const uint32_t lpc_dev = PCI_DEV(0, LPC_DEV, LPC_FUNC);

	/* Memory Mapped IO registers. */
	reg = PMC_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PBASE, reg);
	reg = IO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IOBASE, reg);
	reg = ILB_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IBASE, reg);
	reg = SPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, SBASE, reg);
	reg = MPHY_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, MPBASE, reg);
	reg = PUNIT_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PUBASE, reg);
	reg = RCBA_BASE_ADDRESS | 1;
	pci_write_config32(lpc_dev, RCBA, reg);

	/* IO Port Registers. */
	reg = ACPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, ABASE, reg);
	reg = GPIO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, GBASE, reg);
}

static void spi_init(void)
{
	void *scs = (void *)(SPI_BASE_ADDRESS + SCS);
	void *bcr = (void *)(SPI_BASE_ADDRESS + BCR);
	uint32_t reg;

	/* Disable generating SMI when setting WPD bit. */
	write32(scs, read32(scs) & ~SMIWPEN);
	/*
	 * Enable caching and prefetching in the SPI controller. Disable
	 * the SMM-only BIOS write and set WPD bit.
	 */
	reg = (read32(bcr) & ~SRC_MASK) | SRC_CACHE_PREFETCH | BCR_WPD;
	reg &= ~EISS;
	write32(bcr, reg);
}

static struct chipset_power_state power_state CAR_GLOBAL;

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;
	struct chipset_power_state *ps_car;

	ps_car = car_get_var_ptr(&power_state);
	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, ps_car, sizeof(*ps_cbmem));
}
ROMSTAGE_CBMEM_INIT_HOOK(migrate_power_state);

struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	ps->gpe0_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS);
	ps->gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN);
	ps->tco_sts = inl(ACPI_BASE_ADDRESS + TCO_STS);
	ps->prsts = read32((void *)(PMC_BASE_ADDRESS + PRSTS));
	ps->gen_pmcon1 = read32((void *)(PMC_BASE_ADDRESS + GEN_PMCON1));
	ps->gen_pmcon2 = read32((void *)(PMC_BASE_ADDRESS + GEN_PMCON2));

	ps->prev_sleep_state = chipset_prev_sleep_state(ps);

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
		ps->pm1_sts, ps->pm1_en, ps->pm1_cnt);
	printk(BIOS_DEBUG, "gpe0_sts: %08x gpe0_en: %08x tco_sts: %08x\n",
		ps->gpe0_sts, ps->gpe0_en, ps->tco_sts);
	printk(BIOS_DEBUG, "prsts: %08x gen_pmcon1: %08x gen_pmcon2: %08x\n",
		ps->prsts, ps->gen_pmcon1, ps->gen_pmcon2);
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);
	return ps;
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
		outl(ps->pm1_cnt & ~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
	}

	if (ps->gen_pmcon1 & (PWR_FLR | SUS_PWR_FLR))
		prev_sleep_state = ACPI_S5;

	return prev_sleep_state;
}

/* SOC initialization before the console is enabled */
void car_soc_pre_console_init(void)
{
	/* Early chipset initialization */
	program_base_addresses();
	tco_disable();
}

/* SOC initialization after console is enabled */
void car_soc_post_console_init(void)
{
	/* Continue chipset initialization */
	set_max_freq();
	spi_init();

	lpc_init();
}

/* SOC initialization after RAM is enabled */
void soc_after_ram_init(struct romstage_params *params)
{
	u32 value;

	/* Make sure that E0000 and F0000 are RAM */
	printk(BIOS_DEBUG, "Disable ROM shadow below 1MB.\n");
	value = iosf_bunit_read(BUNIT_BMISC);
	value |= 3;
	iosf_bunit_write(BUNIT_BMISC, value);
}

/* Initialize the UPD parameters for MemoryInit */
void soc_memory_init_params(struct romstage_params *params,
			    MEMORY_INIT_UPD *upd)
{
	const struct device *dev;
	const struct soc_intel_braswell_config *config;

	/* Set the parameters for MemoryInit */
	dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));

	if (!dev) {
		printk(BIOS_ERR,
			"Error! Device (PCI:0:%02x.%01x) not found, "
			"soc_memory_init_params!\n", LPC_DEV, LPC_FUNC);
		return;
	}

	config = dev->chip_info;
	printk(BIOS_DEBUG, "Updating UPD values for MemoryInit\n");
	upd->PcdMrcInitTsegSize = IS_ENABLED(CONFIG_HAVE_SMI_HANDLER) ?
		config->PcdMrcInitTsegSize : 0;
	upd->PcdMrcInitMmioSize = config->PcdMrcInitMmioSize;
	upd->PcdMrcInitSpdAddr1 = config->PcdMrcInitSpdAddr1;
	upd->PcdMrcInitSpdAddr2 = config->PcdMrcInitSpdAddr2;
	upd->PcdIgdDvmt50PreAlloc = config->PcdIgdDvmt50PreAlloc;
	upd->PcdApertureSize = config->PcdApertureSize;
	upd->PcdGttSize = config->PcdGttSize;
	upd->PcdLegacySegDecode = config->PcdLegacySegDecode;
	upd->PcdDvfsEnable = config->PcdDvfsEnable;
	upd->PcdCaMirrorEn = config->PcdCaMirrorEn;
}

void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new)
{
	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");
	fsp_display_upd_value("PcdMrcInitTsegSize", 2,
		old->PcdMrcInitTsegSize, new->PcdMrcInitTsegSize);
	fsp_display_upd_value("PcdMrcInitMmioSize", 2,
		old->PcdMrcInitMmioSize, new->PcdMrcInitMmioSize);
	fsp_display_upd_value("PcdMrcInitSpdAddr1", 1,
		old->PcdMrcInitSpdAddr1, new->PcdMrcInitSpdAddr1);
	fsp_display_upd_value("PcdMrcInitSpdAddr2", 1,
		old->PcdMrcInitSpdAddr2, new->PcdMrcInitSpdAddr2);
	fsp_display_upd_value("PcdMemChannel0Config", 1,
		old->PcdMemChannel0Config, new->PcdMemChannel0Config);
	fsp_display_upd_value("PcdMemChannel1Config", 1,
		old->PcdMemChannel1Config, new->PcdMemChannel1Config);
	fsp_display_upd_value("PcdMemorySpdPtr", 4,
		old->PcdMemorySpdPtr, new->PcdMemorySpdPtr);
	fsp_display_upd_value("PcdIgdDvmt50PreAlloc", 1,
		old->PcdIgdDvmt50PreAlloc, new->PcdIgdDvmt50PreAlloc);
	fsp_display_upd_value("PcdApertureSize", 1,
		old->PcdApertureSize, new->PcdApertureSize);
	fsp_display_upd_value("PcdGttSize", 1,
		old->PcdGttSize, new->PcdGttSize);
	fsp_display_upd_value("PcdLegacySegDecode", 1,
		old->PcdLegacySegDecode, new->PcdLegacySegDecode);
	fsp_display_upd_value("PcdDvfsEnable", 1,
		old->PcdDvfsEnable, new->PcdDvfsEnable);
}
