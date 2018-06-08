/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
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

#include <arch/acpi.h>
#include <arch/io.h>
#include <compiler.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/pmc.h>
#include <soc/pci_devs.h>

/* SoC overrides */

/* Fill up PMC resource structure inside SoC directory */
__weak int pmc_soc_get_resources(
		struct pmc_resource_config *cfg)
{
	/* no-op */
	return -1;
}

/* SoC override PMC initialization */
__weak void pmc_soc_init(struct device *dev)
{
	/* no-op */
}

static void pch_pmc_add_new_resource(struct device *dev,
		uint8_t offset, uintptr_t base, size_t size,
		unsigned long flags)
{
	struct resource *res;
	res = new_resource(dev, offset);
	res->base = base;
	res->size = size;
	res->flags = flags;
}

static void pch_pmc_add_mmio_resources(struct device *dev,
		const struct pmc_resource_config *cfg)
{
	pch_pmc_add_new_resource(dev, cfg->pwrmbase_offset,
			cfg->pwrmbase_addr, cfg->pwrmbase_size,
			IORESOURCE_MEM | IORESOURCE_ASSIGNED |
			IORESOURCE_FIXED | IORESOURCE_RESERVE);
}

static void pch_pmc_add_io_resources(struct device *dev,
		const struct pmc_resource_config *cfg)
{
	pch_pmc_add_new_resource(dev, cfg->abase_offset,
			cfg->abase_addr, cfg->abase_size,
			 IORESOURCE_IO | IORESOURCE_ASSIGNED |
			 IORESOURCE_FIXED);
	if (IS_ENABLED(CONFIG_PMC_INVALID_READ_AFTER_WRITE)) {
		/*
		 * The ACPI IO BAR (offset 0x20) is not PCI compliant. We've
		 * observed cases where the BAR reads back as 0, but the IO
		 * window is open. This also means that it will not respond
		 * to PCI probing.
		 */
		pci_write_config16(dev, cfg->abase_offset, cfg->abase_addr);
		/*
		 * In pci_dev_enable_resources, reading IO SPACE ACCESS bit in
		 * STATUSCOMMAND register does not read back the written
		 * value correctly, hence IO access gets disabled. This is
		 * seen in some PMC devices, hence this code makes sure
		 * IO access is available.
		 */
		dev->command |= PCI_COMMAND_IO;
	}
}

static void pch_pmc_read_resources(struct device *dev)
{
	struct pmc_resource_config pmc_cfg;
	struct pmc_resource_config *config = &pmc_cfg;

	if (pmc_soc_get_resources(config) < 0)
		die("Unable to get PMC controller resource information!");

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_pmc_add_mmio_resources(dev, config);

	/* Add IO resources. */
	pch_pmc_add_io_resources(dev, config);
}

void pmc_set_acpi_mode(void)
{
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER) && !acpi_is_wakeup_s3()) {
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_DISABLE, APM_CNT);
		printk(BIOS_DEBUG, "done.\n");
	}
}

static struct device_operations device_ops = {
	.read_resources		= &pch_pmc_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= &pmc_soc_init,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= &scan_lpc_bus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_LP_PMC,
	PCI_DEVICE_ID_INTEL_SPT_H_PMC,
	PCI_DEVICE_ID_INTEL_KBP_H_PMC,
	PCI_DEVICE_ID_INTEL_APL_PMC,
	PCI_DEVICE_ID_INTEL_GLK_PMC,
	0
};

static const struct pci_driver pch_pmc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
