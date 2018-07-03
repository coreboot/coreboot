/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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


#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <timestamp.h>
#include <cpu/x86/tsc.h>
#include <elog.h>
#include "pch.h"
#include "chip.h"

#if IS_ENABLED(CONFIG_INTEL_LYNXPOINT_LP)
#include "lp_gpio.h"
#else
#include <southbridge/intel/common/gpio.h>
#endif

const struct rcba_config_instruction pch_early_config[] = {
	/* Enable IOAPIC */
	RCBA_SET_REG_16(OIC, 0x0100),
	/* PCH BWG says to read back the IOAPIC enable register */
	RCBA_READ_REG_16(OIC),

	RCBA_END_CONFIG,
};

int pch_is_lp(void)
{
	u8 id = pci_read_config8(PCH_LPC_DEV, PCI_DEVICE_ID + 1);
	return id == PCH_TYPE_LPT_LP;
}

static void pch_enable_bars(void)
{
	/* Setting up Southbridge. In the northbridge code. */
	pci_write_config32(PCH_LPC_DEV, RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCH_LPC_DEV, PMBASE, DEFAULT_PMBASE | 1);
	/* Enable ACPI BAR */
	pci_write_config8(PCH_LPC_DEV, ACPI_CNTL, 0x80);

	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);

	/* Enable GPIO functionality. */
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
}

static void pch_generic_setup(void)
{
	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");
}

uint64_t get_initial_timestamp(void)
{
	tsc_t base_time = {
		.lo = pci_read_config32(PCI_DEV(0, 0x00, 0), 0xdc),
		.hi = pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0)
	};
	return tsc_to_uint64(base_time);
}

static int sleep_type_s3(void)
{
	u32 pm1_cnt;
	u16 pm1_sts;
	int is_s3 = 0;

	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);
	if (pm1_sts & WAK_STS) {
		/* Read PM1_CNT[12:10] to determine which Sx state */
		pm1_cnt = inl(DEFAULT_PMBASE + PM1_CNT);
		if (((pm1_cnt >> 10) & 7) == SLP_TYP_S3) {
			/* Clear SLP_TYPE. */
			outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + PM1_CNT);
			is_s3 = 1;
		}
	}
	return is_s3;
}

void pch_enable_lpc(void)
{
	const struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	const struct southbridge_intel_lynxpoint_config *config = NULL;

	/* Set COM1/COM2 decode range */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO + MC + COM1 + PS/2 Keyboard/Mouse */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN | GAMEL_LPC_EN |
		COMA_LPC_EN | KBC_LPC_EN | MC_LPC_EN;
	pci_write_config16(PCH_LPC_DEV, LPC_EN, lpc_config);

	/* Set up generic decode ranges */
	if (!dev)
		return;
	if (dev->chip_info)
		config = dev->chip_info;
	if (!config)
		return;

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, config->gen4_dec);
}

int early_pch_init(const void *gpio_map,
                   const struct rcba_config_instruction *rcba_config)
{
	int wake_from_s3;

	pch_enable_lpc();

	pch_enable_bars();

#if IS_ENABLED(CONFIG_INTEL_LYNXPOINT_LP)
	setup_pch_lp_gpios(gpio_map);
#else
	setup_pch_gpios(gpio_map);
#endif

	console_init();

	pch_generic_setup();

	/* Enable SMBus for reading SPDs. */
	enable_smbus();

	/* Early PCH RCBA settings */
	pch_config_rcba(pch_early_config);

	/* Mainboard RCBA settings */
	pch_config_rcba(rcba_config);

	wake_from_s3 = sleep_type_s3();

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
	if (!wake_from_s3)
		boot_count_increment();
#endif

	/* Report if we are waking from s3. */
	return wake_from_s3;
}
