/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.  <support@cavium.com>
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
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <console/console.h>
#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/addressmap.h>
#include <soc/cavium/common/pci/chip.h>
#include <assert.h>

#define CAVM_PCCPF_XXX_VSEC_CTL 0x108
#define CAVM_PCCPF_XXX_VSEC_SCTL 0x10c

/*
 * Hide PCI device function on BUS 1 in non secure world.
 */
static void disable_func(unsigned int devfn)
{
	u64 *addr;
	printk(BIOS_DEBUG, "PCI: 01:%02x.%x is secure\n", devfn >> 3,
	       devfn & 7);

	/* disable function */
	addr = (void *)ECAM0_RSLX_SDIS;
	u64 reg = read64(&addr[devfn]);
	reg &= ~3;
	reg |= 2;
	write64(&addr[devfn], reg);
}

/*
 * Show PCI device function on BUS 1 in non secure world.
 */
static void enable_func(unsigned int devfn)
{
	u64 *addr;

	printk(BIOS_DEBUG, "PCI: 01:%02x.%x is insecure\n", devfn >> 3,
	       devfn & 7);

	/* enable function */
	addr = (void *)ECAM0_RSLX_SDIS;
	u64 reg = read64(&addr[devfn]);
	reg &= ~3;
	write64(&addr[devfn], reg);

	addr = (void *)ECAM0_RSLX_NSDIS;
	reg = read64(&addr[devfn]);
	reg &= ~1;
	write64(&addr[devfn], reg);
}

/*
 * Hide PCI device on BUS 0 in non secure world.
 */
static void disable_device(unsigned int dev)
{
	u64 *addr;

	printk(BIOS_DEBUG, "PCI: 00:%02x.0 is secure\n", dev);

	/* disable function */
	addr = (void *)ECAM0_DEVX_SDIS;
	u64 reg = read64(&addr[dev]);
	reg &= ~3;
	write64(&addr[dev], reg);

	addr = (void *)ECAM0_DEVX_NSDIS;
	reg = read64(&addr[dev]);
	reg |= 1;
	write64(&addr[dev], reg);
}

/*
 * Show PCI device on BUS 0 in non secure world.
 */
static void enable_device(unsigned int dev)
{
	u64 *addr;

	printk(BIOS_DEBUG, "PCI: 00:%02x.0 is insecure\n", dev);

	/* enable function */
	addr = (void *)ECAM0_DEVX_SDIS;
	u64 reg = read64(&addr[dev]);
	reg &= ~3;
	write64(&addr[dev], reg);

	addr = (void *)ECAM0_DEVX_NSDIS;
	reg = read64(&addr[dev]);
	reg &= ~1;
	write64(&addr[dev], reg);
}

static void ecam0_read_resources(struct device *dev)
{
	/* There are no dynamic PCI resources on Cavium SoC */
}

static void ecam0_fix_missing_devices(struct bus *link)
{
	size_t i;

	/**
	 * Cavium thinks it's a good idea to violate the PCI spec.
	 * Disabled multi-function PCI devices might have active functions.
	 * Add devices here manually, as coreboot's PCI allocator won't find
	 * them otherwise...
	 */
	for (i = 0; i <= PCI_DEVFN(0x1f, 7); i++) {
		struct device_path pci_path;
		struct device *child;

		pci_path.type = DEVICE_PATH_PCI;
		pci_path.pci.devfn = i;

		child = find_dev_path(link, &pci_path);
		if (!child)
			pci_probe_dev(NULL, link, i);
	}
}

/**
 * Get PCI BAR address from cavium specific extended capability.
 * Use regular BAR if not found in extended capability space.
 *
 * @return The pyhsical address of the BAR, zero on error
 */
static uint64_t get_bar_val(struct device *dev, u8 bar)
{
	size_t cap_offset = pci_find_capability(dev, 0x14);
	uint64_t h, l, ret = 0;
	if (cap_offset) {
		/* Found EA */
		u8 es, bei;
		u8 ne = pci_read_config8(dev, cap_offset + 2) & 0x3f;

		cap_offset += 4;
		while (ne) {
			uint32_t dw0 = pci_read_config32(dev, cap_offset);

			es = dw0 & 7;
			bei = (dw0 >> 4) & 0xf;
			if (bei == bar) {
				h = 0;
				l = pci_read_config32(dev, cap_offset + 4);
				if (l & 2)
					h = pci_read_config32(dev,
							      cap_offset + 12);
				ret = (h << 32) | (l & ~0xfull);
				break;
			}
			cap_offset += (es + 1) * 4;
			ne--;
		}
	} else {
		h = 0;
		l = pci_read_config32(dev, bar * 4 + PCI_BASE_ADDRESS_0);
		if (l & 4)
			h = pci_read_config32(dev, bar * 4 + PCI_BASE_ADDRESS_0
					      + 4);
		ret = (h << 32) | (l & ~0xfull);
	}
	return ret;
}

/**
 * pci_enable_msix - configure device's MSI-X capability structure
 * @dev: pointer to the pci_dev data structure of MSI-X device function
 * @entries: pointer to an array of MSI-X entries
 * @nvec: number of MSI-X irqs requested for allocation by device driver
 *
 * Setup the MSI-X capability structure of device function with the number
 * of requested irqs upon its software driver call to request for
 * MSI-X mode enabled on its hardware device function. A return of zero
 * indicates the successful configuration of MSI-X capability structure.
 * A return of < 0 indicates a failure.
 * Or a return of > 0 indicates that driver request is exceeding the number
 * of irqs or MSI-X vectors available. Driver should use the returned value to
 * re-send its request.
 **/
static size_t ecam0_pci_enable_msix(struct device *dev,
				    struct msix_entry *entries, size_t nvec)
{
	struct msix_entry *msixtable;
	u32 offset;
	u8 bar_idx;
	u64 bar;
	size_t nr_entries;
	size_t i;
	u16 control;

	if (!entries) {
		printk(BIOS_ERR, "%s: No entries specified\n", __func__);
		return -1;
	}

	const size_t pos = pci_find_capability(dev, PCI_CAP_ID_MSIX);
	if (!pos) {
		printk(BIOS_ERR, "%s: Device not MSI-X capable\n",
		       dev_path(dev));
		return -1;
	}
	nr_entries = pci_msix_table_size(dev);
	if (nvec > nr_entries) {
		printk(BIOS_ERR, "ERROR: %s: Specified to many table entries\n",
		       dev_path(dev));
		return nr_entries;
	}

	/* Ensure MSI-X is disabled while it is set up */
	control = pci_read_config16(dev, pos + PCI_MSIX_FLAGS);
	control &= ~PCI_MSIX_FLAGS_ENABLE;
	pci_write_config16(dev, pos + PCI_MSIX_FLAGS, control);

	/* Find MSI-X table region */
	offset = 0;
	bar_idx = 0;
	if (pci_msix_table_bar(dev, &offset, &bar_idx)) {
		printk(BIOS_ERR, "ERROR: %s: Failed to find MSI-X entry\n",
		       dev_path(dev));
		return -1;
	}
	bar = get_bar_val(dev, bar_idx);
	if (!bar) {
		printk(BIOS_ERR, "ERROR: %s: Failed to find MSI-X bar\n",
		       dev_path(dev));
		return -1;
	}
	msixtable = (struct msix_entry *)((void *)bar + offset);

	/*
	 * Some devices require MSI-X to be enabled before we can touch the
	 * MSI-X registers.  We need to mask all the vectors to prevent
	 * interrupts coming in before they're fully set up.
	 */
	control |= PCI_MSIX_FLAGS_MASKALL | PCI_MSIX_FLAGS_ENABLE;
	pci_write_config16(dev, pos + PCI_MSIX_FLAGS, control);

	for (i = 0; i < nvec; i++) {
		write64(&msixtable[i].addr, entries[i].addr);
		write32(&msixtable[i].data, entries[i].data);
		write32(&msixtable[i].vec_control, entries[i].vec_control);
	}

	control &= ~PCI_MSIX_FLAGS_MASKALL;
	pci_write_config16(dev, pos + PCI_MSIX_FLAGS, control);

	return 0;
}

static void ecam0_init(struct device *dev)
{
	struct soc_cavium_common_pci_config *config;
	struct device *child, *child_last;
	size_t i;
	u32 reg32;

	printk(BIOS_INFO, "ECAM0: init\n");
	const struct device *bridge = dev_find_slot(0, PCI_DEVFN(1, 0));
	if (!bridge) {
		printk(BIOS_INFO, "ECAM0: ERROR: PCI 00:01.0 not found.\n");
		return;
	}
	/**
	 * Search for missing devices on BUS 1.
	 * Only required for ARI capability programming.
	 */
	ecam0_fix_missing_devices(bridge->link_list);

	/* Program secure ARI capability on bus 1 */
	child_last = NULL;
	for (i = 0; i <= PCI_DEVFN(0x1f, 7); i++) {
		child = dev_find_slot(bridge->link_list->secondary, i);
		if (!child || !child->enabled)
			continue;

		if (child_last) {
			/* Program ARI capability of the previous device */
			reg32 = pci_read_config32(child_last,
						  CAVM_PCCPF_XXX_VSEC_SCTL);
			reg32 &= ~(0xffU << 24);
			reg32 |= child->path.pci.devfn << 24;
			pci_write_config32(child_last, CAVM_PCCPF_XXX_VSEC_SCTL,
					   reg32);
		}
		child_last = child;
	}

	/* Program insecure ARI capability on bus 1 */
	child_last = NULL;
	for (i = 0; i <= PCI_DEVFN(0x1f, 7); i++) {
		child = dev_find_slot(bridge->link_list->secondary, i);
		if (!child)
			continue;
		config = child->chip_info;
		if (!child->enabled || (config && config->secure))
			continue;

		if (child_last) {
			/* Program ARI capability of the previous device */
			reg32 = pci_read_config32(child_last,
						  CAVM_PCCPF_XXX_VSEC_CTL);
			reg32 &= ~(0xffU << 24);
			reg32 |= child->path.pci.devfn << 24;
			pci_write_config32(child_last, CAVM_PCCPF_XXX_VSEC_CTL,
					   reg32);
		}
		child_last = child;
	}

	/* Enable / disable devices on bus 0 */
	for (i = 0; i <= 0x1f; i++) {
		child = dev_find_slot(0, PCI_DEVFN(i, 0));
		config = child ? child->chip_info : NULL;
		if (child && child->enabled && config && !config->secure)
			enable_device(i);
		else
			disable_device(i);
	}

	/* Enable / disable devices and functions on bus 1 */
	for (i = 0; i <= PCI_DEVFN(0x1f, 7); i++) {
		child = dev_find_slot(bridge->link_list->secondary, i);
		config = child ? child->chip_info : NULL;
		if (child && child->enabled &&
		    ((config && !config->secure) || !config))
			enable_func(i);
		else
			disable_func(i);
	}

	/* Apply IRQ on PCI devices */
	/* UUA */
	for (i = 0; i < 4; i++) {
		child = dev_find_slot(bridge->link_list->secondary,
				      PCI_DEVFN(8, i));
		if (!child)
			continue;

		struct msix_entry entry[2] = {
			{.addr = CAVM_GICD_SETSPI_NSR, .data = 37 + i},
			{.addr = CAVM_GICD_CLRSPI_NSR, .data = 37 + i},
		};

		ecam0_pci_enable_msix(child, entry, 2);
	}

	printk(BIOS_INFO, "ECAM0: done\n");
}

struct device_operations pci_domain_ops_ecam0 = {
	.set_resources    = NULL,
	.enable_resources = NULL,
	.read_resources   = ecam0_read_resources,
	.init             = ecam0_init,
	.scan_bus         = pci_domain_scan_bus,
};
