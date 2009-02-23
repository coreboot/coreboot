/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Artec Design LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <statictree.h>
#include "cs5536.h"

/**
 * Enables the FLASH PCI header when NAND device existing in mainboard device
 * tree. Used when the mainboard has a FLASH part instead of an IDE drive and
 * that fact is expressed in the mainboard device tree.
 * Must be called after VSA init but before PCI scans to enable the flash
 * PCI device header early enough - that is .phase2_fixup of the device.
 *
 * @param dev The device.
 */
static void nand_phase2(struct device *dev)
{
	if (dev->enabled) {
		struct southbridge_amd_cs5536_nand_config *nand;
		struct msr msr;

		/* Set up timings */
		nand = (struct southbridge_amd_cs5536_nand_config *)dev->device_configuration;
		msr.hi = 0x0;

		if (nand->nandf_data) {
			msr.lo = nand->nandf_data;
			wrmsr(MDD_NANDF_DATA, msr);
			printk(BIOS_DEBUG, "NANDF_DATA set to 0x%08x\n", msr.lo);
		}
		if (nand->nandf_ctl) {
			msr.lo = nand->nandf_ctl;
			wrmsr(MDD_NANDF_CTL, msr);
			printk(BIOS_DEBUG, "NANDF_CTL set to 0x%08x\n", msr.lo);
		}

		/* Tell VSA to use FLASH PCI header. Not IDE header. */
		hide_vpci(0x800079C4);
	}
}

static void nand_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* All memory accesses in the range of 0xF0000000 - 0xFFFFFFFF routed to
	 * Diverse Integration Logic (DIVIL) get always sent to the device inside
	 * DIVIL as set in DIVIL_BALL_OPTS PRI_BOOT_LOC and SEC_BOOT_LOC bits
	 * (see CS5536 data book chapter 6.6.2.10 DIVIL_BALL_OPTS PRI_BOOT_LOC
	 * description).
	 * The virtual PCI address limit test gives us a false upper limit of
	 * 0xFFFFFFFF for this device, but we do not want NAND Flash to be using
	 * memory addresses 0xF0000000 and above as those accesses would end up
	 * somewhere else instead. Therefore if VSA2 gave us a MMIO resource for
	 * NAND Flash, patch this (fixed) resources higher bound to 0xEFFFFFFF.
	 */
	if ((dev->resources >= 1) && (dev->resource[0].flags & IORESOURCE_MEM) &&
		(dev->resource[0].limit > 0xefffffff))
		dev->resource[0].limit = 0xefffffff;
}

struct device_operations cs5536_nand = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			 .device = PCI_DEVICE_ID_AMD_CS5536_FLASH}}},
	.constructor		 = default_device_constructor,
	.phase2_fixup		 = nand_phase2,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = nand_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = 0, /* No Option ROMs */
	.ops_pci		 = &pci_dev_ops_pci,
};
