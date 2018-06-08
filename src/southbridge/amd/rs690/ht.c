/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <lib.h>
#include <arch/acpi.h>
#include "rs690.h"

static void ht_dev_set_resources(struct device *dev)
{
#if IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)
	unsigned reg;
	struct device *k8_f1;
	resource_t rbase, rend;
	u32 base, limit;
	struct resource *resource;

	printk(BIOS_DEBUG,"%s %s\n", dev_path(dev), __func__);

	resource = probe_resource(dev, 0x1C);
	if (resource) {
		set_nbmisc_enable_bits(dev, 0x0, 1 << 3, 0 << 3); // make bar3 visible
		set_nbcfg_enable_bits(dev, 0x7C, 1 << 30, 1 << 30);	/* Enables writes to the BAR3 register. */
		set_nbcfg_enable_bits(dev, 0x84, 7 << 16, 0 << 16); // program bus range: 255 busses
		pci_write_config32(dev, 0x1C, resource->base);
		/* Enable MMCONFIG decoding. */
		set_htiu_enable_bits(dev, 0x32, 1 << 28, 1 << 28);	/* PCIEMiscInit */
		set_nbcfg_enable_bits(dev, 0x7C, 1 << 30, 0 << 30);	/* Disable writes to the BAR3 register. */
		set_nbmisc_enable_bits(dev, 0x0, 1 << 3, 1 << 3); // hide bar 3

		// setup resource nonposted in k8 mmio
		/* Get the base address */
		rbase = resource->base;
		/* Get the limit (rounded up) */
		rend  = resource_end(resource);
		printk(BIOS_DEBUG,"%s: %s[0x1C] base = %0llx limit = %0llx\n", __func__, dev_path(dev), rbase, rend);
		k8_f1 = dev_find_slot(0,PCI_DEVFN(0x18,1));
		// find a not assigned resource
		for ( reg = 0xb8; reg >= 0x80; reg -= 8 ) {
			base = pci_read_config32(k8_f1,reg);
			limit = pci_read_config32(k8_f1,reg+4);
			if ( !(base & 3) ) break; // found a not assigned resource
		}
		if ( !(base & 3) ) {
			u32 sblk;
			struct device *k8_f0 =
				dev_find_slot(0, PCI_DEVFN(0x18, 0));
			/* Remember this resource has been stored. */
			resource->flags |= IORESOURCE_STORED;
			report_resource_stored(dev, resource, " <mmconfig>");
			/* Get SBLink value (HyperTransport I/O Hub Link ID). */
			sblk = (pci_read_config32(k8_f0, 0x64) >> 8) & 0x3;
			base  &= 0x000000f0;
			base  |= ((rbase >> 8) & 0xffffff00);
			base  |= 3;
			limit &= 0x00000048;
			limit |= ((rend >> 8) & 0xffffff00);
			limit |= (sblk << 4);
			limit |= (1 << 7);
			printk(BIOS_INFO, "%s <- index %x base %04x limit %04x\n", dev_path(k8_f1), reg, base, limit);
			pci_write_config32(k8_f1, reg+4, limit);
			pci_write_config32(k8_f1, reg, base);
		}
	}
#endif
	pci_dev_set_resources(dev);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
#if IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)
	struct resource *res;
	resource_t mmconf_base = EXT_CONF_BASE_ADDRESS; // default

	struct device *dev = dev_find_slot(0,PCI_DEVFN(0,0));
	// we report mmconf base
	res = probe_resource(dev, 0x1C);
	if ( res )
		mmconf_base = res->base;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current, mmconf_base, 0x0, 0x0, 0x1f); // Fix me: should i reserve 255 busses ?
#endif
	return current;
}

static void ht_dev_read_resources(struct device *dev)
{
#if IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)
	struct resource *res;

	printk(BIOS_DEBUG,"%s %s\n", dev_path(dev), __func__);
	set_nbmisc_enable_bits(dev, 0x0, 1 << 3, 1 << 3); // hide bar 3
#endif

	pci_dev_read_resources(dev);

#if IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)
	/* Add an MMCONFIG resource. */
	res = new_resource(dev, 0x1C);
	res->base = EXT_CONF_BASE_ADDRESS;
	res->size = 256 * 1024 * 1024; // 256 busses, 1MB memory space each
	res->align = log2(res->size);
	res->gran = log2(res->size);
	res->limit = 0xffffffffffffffffULL;	/* 64bit */
	res->flags = IORESOURCE_FIXED | IORESOURCE_MEM | IORESOURCE_PCI64 |
		IORESOURCE_ASSIGNED | IORESOURCE_RESERVE;

	compact_resources(dev);
#endif
}

/* for UMA internal graphics */
void avoid_lpc_dma_deadlock(struct device *nb_dev, struct device *sb_dev)
{
	struct device *k8_f0;
	u8 reg;

	k8_f0 = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	set_nbcfg_enable_bits(k8_f0, 0x68, 3 << 21, 1 << 21);

	reg = nbpcie_p_read_index(sb_dev, 0x10);
	reg |= 0x100;		/* bit9=1 */
	nbpcie_p_write_index(sb_dev, 0x10, reg);

	reg = nbpcie_p_read_index(nb_dev, 0x10);
	reg |= 0x100;		/* bit9=1 */
	nbpcie_p_write_index(nb_dev, 0x10, reg);

	/* Enable NP protocol over PCIE for memory-mapped writes targeting LPC
	 * Set this bit to avoid a deadlock condition. */
	reg = htiu_read_index(nb_dev, 0x6);
	reg |= 0x1000000;	/* bit26 */
	htiu_write_index(nb_dev, 0x6, reg);
}

static void pcie_init(struct device *dev)
{
	/* Enable pci error detecting */
	u32 dword;

	printk(BIOS_INFO, "pcie_init in rs690_ht.c\n");

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* System error enable */
	dword |= (1 << 30);	/* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

	/*
	 * 1 is APIC enable
	 * 18 is enable nb to accept A4 interrupt request from SB.
	 */
	dword = pci_read_config32(dev, 0x4C);
	dword |= 1 << 1 | 1 << 18;	/* Clear possible errors */
	pci_write_config32(dev, 0x4C, dword);
}

static void ht_dev_set_subsystem(struct device *dev, unsigned vendor,
				 unsigned device)
{
	pci_write_config32(dev, 0x50,  ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = ht_dev_set_subsystem,
};

static struct device_operations ht_ops = {
	.read_resources = ht_dev_read_resources,
	.set_resources = ht_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = pcie_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver ht_driver __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_RS690_HT,
};
