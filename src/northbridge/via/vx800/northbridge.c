/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

/*
    part of this file is from cx700 port, part of is from cn700 port,
   */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include "vx800.h"

/* !!FIXME!!  This was meant to be a CONFIG option */
#define VIACONFIG_TOP_SM_SIZE_MB 32	// Set frame buffer 32M for default
/* !!FIXME!!  I declared this to fix the build. */
u8 acpi_sleep_type = 0;

static void memctrl_init(struct device *dev)
{
/*
  set VGA in uma_ram_setting.c, not in this function.
*/
}

static const struct device_operations memctrl_operations = {
	.read_resources = DEVICE_NOOP,
	.init = memctrl_init,
};

static const struct pci_driver memctrl_driver __pci_driver = {
	.ops = &memctrl_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX855_MEMCTRL,
};

static void pci_domain_set_resources(struct device *dev)
{
	/*
	 * the order is important to find the correct RAM size.
	 */
	u8 ramregs[] = { 0x43, 0x42, 0x41, 0x40 };
	struct device *mc_dev;
	u32 pci_tolm;
	u8 reg;

	printk(BIOS_SPEW, "Entering vx800 pci_domain_set_resources.\n");

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				 PCI_DEVICE_ID_VIA_VX855_MEMCTRL, 0);

	if (mc_dev) {
		unsigned long tomk, tolmk;
		unsigned char rambits;
		u8 i, idx;

		/*
		 * once the register value is not zero, the ramsize is
		 * this register's value multiply 64 * 1024 * 1024
		 */
		for (rambits = 0, i = 0; i < ARRAY_SIZE(ramregs); i++) {
			rambits = pci_read_config8(mc_dev, ramregs[i]);
			if (rambits != 0)
				break;
		}
/*
Get memory size and frame buffer from northbridge's registers.
if register with invalid value we set frame buffer size to 32M for default, but it won't happen.
*/
		reg = pci_read_config8(mc_dev, 0xa1);
		reg &= 0x70;
		reg = reg >> 4;
		/* TOP 1M SM Memory */
		if (reg == 0x0)
			tomk = (((rambits << 6) - 32 - VIACONFIG_TOP_SM_SIZE_MB) * 1024);	// Set frame buffer 32M for default
		else
			tomk =
			    (((rambits << 6) - (4 << reg) -
			      VIACONFIG_TOP_SM_SIZE_MB) * 1024);

		printk(BIOS_SPEW, "tomk is 0x%lx\n", tomk);
		/* Compute the Top Of Low Memory, in Kb */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory. */
			tolmk = tomk;
		}
		/* Report the memory regions */
		idx = 10;
		/* TODO: Hole needed? */
		ram_resource(dev, idx++, 0, 640);	/* first 640k */
		/* Leave a hole for vga, 0xa0000 - 0xc0000 */
		ram_resource(dev, idx++, 768, (tolmk - 768));
	}
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables = acpi_write_hpet,
#endif
	.enable_resources = NULL,
	.init = NULL,
	.scan_bus = pci_domain_scan_bus,
};

static void cpu_bus_init(struct device *dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = cpu_bus_init,
	.scan_bus = 0,
};

static void enable_dev(struct device *dev)
{
	printk(BIOS_SPEW, "In VX800 enable_dev for device %s.\n", dev_path(dev));

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_vx800_ops = {
	CHIP_NAME("VIA VX800 Chipset")
	    .enable_dev = enable_dev,
};
