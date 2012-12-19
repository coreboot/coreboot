/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 ChromeOS Authors
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <console/console.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include <elog.h>
#include "pch.h"

#if CONFIG_INTEL_LYNXPOINT_LP
#include "lp_gpio.h"
#else
#include "gpio.h"
#endif

static void pch_enable_bars(void)
{
	/* Setting up Southbridge. In the northbridge code. */
	pci_write_config32(PCH_LPC_DEV, RCBA, DEFAULT_RCBA | 1);

	pci_write_config32(PCH_LPC_DEV, PMBASE, DEFAULT_PMBASE | 1);
	/* Enable ACPI BAR */
	pci_write_config8(PCH_LPC_DEV, ACPI_CNTL, 0x80);

	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);

	/* Enable GPIO functionality. */
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
}

static void pch_generic_setup(void)
{
	u8 reg8;

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");

	// reset rtc power status
	reg8 = pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3, reg8);
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

static void pch_enable_lpc(void)
{
	device_t dev = PCH_LPC_DEV;

	/* Set COM1/COM2 decode range */
	pci_write_config16(dev, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO + COM1 + PS/2 Keyboard/Mouse */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN | KBC_LPC_EN;
	pci_write_config16(dev, LPC_EN, lpc_config);
}

static void pch_config_rcba(const struct rcba_config_instruction *rcba_config)
{
	const struct rcba_config_instruction *rc;
	u32 value;

	rc = rcba_config;
	while (rc->command != RCBA_COMMAND_END)
	{
		if ((rc->command & RCBA_REG_SIZE_MASK) == RCBA_REG_SIZE_16) {
			switch (rc->command & RCBA_COMMAND_MASK) {
			case RCBA_COMMAND_SET:
				RCBA16(rc->reg) = (u16)rc->or_value;
				break;
			case RCBA_COMMAND_READ:
				(void)RCBA16(rc->reg);
				break;
			case RCBA_COMMAND_RMW:
				value = RCBA16(rc->reg);
				value &= rc->mask;
				value |= rc->or_value;
				RCBA16(rc->reg) = (u16)value;
				break;
			}
		} else {
			switch (rc->command & RCBA_COMMAND_MASK) {
			case RCBA_COMMAND_SET:
				RCBA32(rc->reg) = rc->or_value;
				break;
			case RCBA_COMMAND_READ:
				(void)RCBA32(rc->reg);
				break;
			case RCBA_COMMAND_RMW:
				value = RCBA32(rc->reg);
				value &= rc->mask;
				value |= rc->or_value;
				RCBA32(rc->reg) = value;
				break;
			}
		}
		rc++;
	}
}

int early_pch_init(const void *gpio_map,
                   const struct rcba_config_instruction *rcba_config)
{
	int wake_from_s3;

	pch_enable_lpc();

	pch_enable_bars();

#if CONFIG_INTEL_LYNXPOINT_LP
	setup_pch_lp_gpios(gpio_map);
#else
	setup_pch_gpios(gpio_map);
#endif

	console_init();

	pch_generic_setup();

	/* Enable SMBus for reading SPDs. */
	enable_smbus();

	pch_config_rcba(rcba_config);

	wake_from_s3 = sleep_type_s3();

#if CONFIG_ELOG_BOOT_COUNT
	if (!wake_from_s3)
		boot_count_increment();
#endif

	/* Report if we are waking from s3. */
	return wake_from_s3;
}
