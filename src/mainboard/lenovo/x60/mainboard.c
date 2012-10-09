/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include <northbridge/intel/i945/i945.h>
#include <pc80/mc146818rtc.h>
#include "dock.h"
#include <arch/x86/include/arch/acpigen.h>

static acpi_cstate_t cst_entries[] = {
	{ 1,  1, 1000, { 0x7f, 1, 2, { 0 }, 1, 0 } },
	{ 2,  1,  500, { 0x01, 8, 0, { 0 }, DEFAULT_PMBASE + LV2, 0 } },
	{ 2, 17,  250, { 0x01, 8, 0, { 0 }, DEFAULT_PMBASE + LV3, 0 } },
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

static void mainboard_enable(device_t dev)
{
	device_t dev0, idedev, sdhci_dev;
	u8 defaults_loaded = 0;

	ec_clr_bit(0x03, 2);

	if (inb(0x164c) & 0x08) {
		ec_set_bit(0x03, 2);
		ec_write(0x0c, 0x88);
	}
	/* If we're resuming from suspend, blink suspend LED */
	dev0 = dev_find_slot(0, PCI_DEVFN(0,0));
	if (dev0 && pci_read_config32(dev0, SKPAD) == SKPAD_ACPI_S3_MAGIC)
		ec_write(0x0c, 0xc7);

	idedev = dev_find_slot(0, PCI_DEVFN(0x1f,1));
	if (idedev && idedev->chip_info && dock_ultrabay_device_present()) {
		struct southbridge_intel_i82801gx_config *config = idedev->chip_info;
		config->ide_enable_primary = 1;
		/* enable Ultrabay power */
		outb(inb(0x1628) | 0x01, 0x1628);
		ec_write(0x0c, 0x84);
	} else {
		/* disable Ultrabay power */
		outb(inb(0x1628) & ~0x01, 0x1628);
		ec_write(0x0c, 0x04);
	}

	/* Set SDHCI write protect polarity "SDWPPol" */
	sdhci_dev = dev_find_device(PCI_VENDOR_ID_RICOH, PCI_DEVICE_ID_RICOH_R5C822, 0);
	if (sdhci_dev) {
		if (pci_read_config8(sdhci_dev, 0xfa) != 0x20) {
			/* unlock */
			pci_write_config8(sdhci_dev, 0xf9, 0xfc);
			/* set SDWPPol, keep CLKRUNDis, SDPWRPol clear */
			pci_write_config8(sdhci_dev, 0xfa, 0x20);
			/* restore lock */
			pci_write_config8(sdhci_dev, 0xf9, 0x00);
		}
	}

	if (get_option(&defaults_loaded, "cmos_defaults_loaded") < 0) {
		printk(BIOS_INFO, "failed to get cmos_defaults_loaded");
		defaults_loaded = 0;
	}

	if (!defaults_loaded) {
		printk(BIOS_INFO, "Restoring CMOS defaults\n");
		set_option("tft_brightness", &(u8[]){ 0xff });
		set_option("volume", &(u8[]){ 0x03 });
		/* set baudrate to 115200 baud */
		set_option("baud_rate", &(u8[]){ 0x00 });
		/* set default debug_level (DEFAULT_CONSOLE_LOGLEVEL starts at 1) */
		set_option("debug_level", &(u8[]) { CONFIG_DEFAULT_CONSOLE_LOGLEVEL+1 });
		set_option("cmos_defaults_loaded", &(u8[]){ 0x01 });
	}
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

