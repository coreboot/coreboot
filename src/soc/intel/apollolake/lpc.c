/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <soc/acpi.h>
#include <soc/lpc.h>
#include <soc/pm.h>
#include <vboot/vbnv.h>

#include "chip.h"

/*
 * SCOPE:
 * The purpose of this driver is to eliminate manual resource allocation for
 * devices under the LPC bridge.
 *
 * BACKGROUND:
 * The resource allocator reserves IO and memory resources to devices on the
 * LPC bus, but it is up to the hardware driver to make sure that those
 * resources are decoded to the LPC bus. This is what this driver does.
 *
 * THEORY OF OPERATION:
 * The .scan_bus member of the driver's ops will scan the static device tree
 * (devicetree.cb) and invoke drivers of devices on the LPC bus. This creates
 * a list of child devices, along with their resources. set_child_resources()
 * parses that list and looks for resources needed by the child devices. It
 * opens up IO and memory windows as needed.
 */

static void rtc_init(void)
{
	int rtc_fail;
	const struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);

	if (!ps) {
		printk(BIOS_ERR, "Could not find power state in cbmem, RTC init aborted\n");
		return;
	}

	rtc_fail = !!(ps->gen_pmcon1 & RPS);
	/* Ensure the date is set including century byte. */
	cmos_check_update_date();
	if (IS_ENABLED(CONFIG_VBOOT_VBNV_CMOS))
		init_vbnv_cmos(rtc_fail);
	else
		cmos_init(rtc_fail);
}

static void lpc_init(struct device *dev)
{
	uint8_t scnt;
	struct soc_intel_apollolake_config *cfg;

	cfg = dev->chip_info;
	if (!cfg) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}

	scnt = pci_read_config8(dev, REG_SERIRQ_CTL);
	scnt &= ~(SCNT_EN | SCNT_MODE);
	if (cfg->serirq_mode == SERIRQ_QUIET)
		scnt |= SCNT_EN;
	else if (cfg->serirq_mode == SERIRQ_CONTINUOUS)
		scnt |= SCNT_EN | SCNT_MODE;
	pci_write_config8(dev, REG_SERIRQ_CTL, scnt);

	/* Initialize RTC */
	rtc_init();
}

static void soc_lpc_add_io_resources(device_t dev)
{
	struct resource *res;

	/* Add the default claimed legacy IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void soc_lpc_read_resources(device_t dev)
{
	/* Get the PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add IO resources to LPC. */
	soc_lpc_add_io_resources(dev);
}

static void set_child_resources(struct device *dev);

static void loop_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next) {

		if (res->flags & IORESOURCE_IO)
			lpc_open_pmio_window(res->base, res->size);

		if (res->flags & IORESOURCE_MEM) {
			/* Check if this is already decoded. */
			if (lpc_fits_fixed_mmio_window(res->base, res->size))
				continue;

			lpc_open_mmio_window(res->base, res->size);
		}

	}
	set_child_resources(dev);
}

/*
 * Loop through all the child devices' resources, and open up windows to the
 * LPC bus, as appropriate.
 */
static void set_child_resources(struct device *dev)
{
	struct bus *link;
	struct device *child;

	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling)
			loop_resources(child);
	}
}

static void set_resources(device_t dev)
{
	pci_dev_set_resources(dev);

	/* Close all previously opened windows and allocate from scratch. */
	lpc_close_pmio_windows();
	/* Now open up windows to devices which have declared resources. */
	set_child_resources(dev);
}

static struct device_operations device_ops = {
	.read_resources = &soc_lpc_read_resources,
	.set_resources = set_resources,
	.enable_resources = &pci_dev_enable_resources,
	.write_acpi_tables = southbridge_write_acpi_tables,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.init = lpc_init,
	.scan_bus = scan_lpc_bus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_LPC,
	PCI_DEVICE_ID_INTEL_GLK_LPC,
	0,
};

static const struct pci_driver soc_lpc __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
