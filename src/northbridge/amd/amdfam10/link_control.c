/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,  MA 02110-1301 USA
 */

/* Configure various power control registers, including processor
 * boost support.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <lib.h>
#include <cpu/amd/model_10xxx_rev.h>

#include "amdfam10.h"

static inline uint8_t is_fam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

static void nb_control_init(struct device *dev)
{
	uint32_t dword;

	printk(BIOS_DEBUG, "NB: Function 4 Link Control.. ");

	if (is_fam15h()) {
		/* Enable APM */
		dword = pci_read_config32(dev, 0x15c);
		dword |= (0x1 << 7);			/* ApmMasterEn = 1 */
		pci_write_config32(dev, 0x15c, dword);
	}

	printk(BIOS_DEBUG, "done.\n");
}


static struct device_operations mcf4_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = nb_control_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static const struct pci_driver mcf4_driver_fam10 __pci_driver = {
	.ops    = &mcf4_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1204,
};

static const struct pci_driver mcf4_driver_fam15 __pci_driver = {
	.ops    = &mcf4_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1604,
};