/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <cpu/amd/powernow.h>
#include <lib.h>
#include "k8t890.h"

static void mmconfig_set_resources(struct device *dev)
{
	struct resource *resource;
	u8 reg;

	resource = find_resource(dev, K8T890_MMCONFIG_MBAR);
	if (resource) {
		report_resource_stored(dev, resource, "<mmconfig>");

		/* Remember this resource has been stored. */
		resource->flags |= IORESOURCE_STORED;
		pci_write_config8(dev, K8T890_MMCONFIG_MBAR,
				  (resource->base >> 28));
		reg = pci_read_config8(dev, 0x60);
		reg |= 0x3;
		/* Enable MMCONFIG decoding. */
		pci_write_config8(dev, 0x60, reg);
	}
	pci_dev_set_resources(dev);
}

static void apic_mmconfig_read_resources(struct device *dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	res = new_resource(dev, 0x40);
	/* NB APIC fixed to this address. */
	res->base = K8T890_APIC_BASE;
	res->size = 256;
	res->limit = res->base + res->size - 1;
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	/* Add an MMCONFIG resource. */
	res = new_resource(dev, K8T890_MMCONFIG_MBAR);
	res->size = 256 * 1024 * 1024;
	res->align = log2(res->size);
	res->gran = log2(res->size);
	res->limit = 0xffffffff;	/* 4G */
	res->flags = IORESOURCE_MEM | IORESOURCE_RESERVE;
}

static void traf_ctrl_enable_generic(struct device *dev)
{
	volatile u32 *apic;
	u32 data;

	/* no device2 redirect, enable just one device behind
	 * bridge device 2 and device 3).
	 */
	pci_write_config8(dev, 0x60, 0x08);

	/* Will enable MMCONFIG later. */
	pci_write_config8(dev, 0x64, 0x23);
	/* No extended RCRB Base Address. */
	pci_write_config8(dev, 0x62, 0x00);

	/* Offset80 ->95 bit 4 in 1 in Award. */

	/* Enable APIC, to K8T890_APIC_BASE. */
	pci_write_config8(dev, 0x41, 0x00);
	pci_write_config8(dev, 0x40, 0x8c);
	/* BT_INTR enable, APIC Nonshare Mode Enable. */
	pci_write_config8(dev, 0x42, 0x5);

	apic = (u32 *)K8T890_APIC_BASE;

	/* Set APIC to FSB transported messages. */
	apic[0] = 3;
	data = apic[4];
	apic[4] = (data & 0xFFFFFE) | 1;

	/* Set APIC ID. */
	apic[0] = 0;
	data = apic[4];
	apic[4] = (data & 0xF0FFFF) | (K8T890_APIC_ID << 24);
}

static void traf_ctrl_enable_k8m890(struct device *dev)
{
	traf_ctrl_enable_generic(dev);
}

static void traf_ctrl_enable_k8t890(struct device *dev)
{
	u8 reg;

	traf_ctrl_enable_generic(dev);

	/* Enable D3F1-D3F3 */
	reg = pci_read_config8(dev, 0x60);
	pci_write_config8(dev, 0x60, 0x80 | reg);
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

static void southbridge_acpi_fill_ssdt_generator(struct device *dev) {
	amd_generate_powernow(0, 0, 0);
	acpigen_write_mainboard_resources("\\_SB.PCI0.MBRS", "_CRS");
}

#endif

static const struct device_operations traf_ctrl_ops_m = {
	.read_resources		= apic_mmconfig_read_resources,
	.set_resources		= mmconfig_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= traf_ctrl_enable_k8m890,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt_generator = southbridge_acpi_fill_ssdt_generator,
#endif
	.ops_pci		= 0,
};

static const struct device_operations traf_ctrl_ops_t = {
	.read_resources		= apic_mmconfig_read_resources,
	.set_resources		= mmconfig_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= traf_ctrl_enable_k8t890,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables      = acpi_write_hpet,
#endif
	.ops_pci		= 0,
};

/* K8X800 chipsets have no APIC; no 800 PCI ids here */

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct device *dev;
	struct resource *res;

	dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_K8T890CE_5, 0);
	if (!dev)
		dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_K8T890CF_5, 0);
	if (!dev)
		dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_K8M890CE_5, 0);
	if (!dev)
		return current;

	res = find_resource(dev, K8T890_MMCONFIG_MBAR);
	if (res) {
		current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)
				current, res->base, 0x0, 0x0, 0xff);
	}
	return current;
}


static const struct pci_driver northbridge_driver_t __pci_driver = {
	.ops	= &traf_ctrl_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_5,
};

static const struct pci_driver northbridge_driver_tcf __pci_driver = {
	.ops	= &traf_ctrl_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CF_5,
};

static const struct pci_driver northbridge_driver_m __pci_driver = {
	.ops	= &traf_ctrl_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M890CE_5,
};
