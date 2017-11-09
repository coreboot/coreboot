/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/spi.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/gspi.h>
#include <intelblocks/spi.h>
#include <soc/ramstage.h>
#include <soc/pci_devs.h>
#include <spi-generic.h>

int spi_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_SPI:
		return 0;
	case PCH_DEVFN_GSPI0:
		return 1;
	case PCH_DEVFN_GSPI1:
		return 2;
	case PCH_DEVFN_GSPI2:
		return 3;
	}
	return -1;
}

int spi_soc_bus_to_devfn(unsigned int bus)
{
	switch (bus) {
	case 0:
		return PCH_DEVFN_SPI;
	case 1:
		return PCH_DEVFN_GSPI0;
	case 2:
		return PCH_DEVFN_GSPI1;
	case 3:
		return PCH_DEVFN_GSPI2;
	}
	return -1;
}

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{ .ctrlr = &fast_spi_flash_ctrlr, .bus_start = 0, .bus_end = 0 },
#if !ENV_SMM
	{ .ctrlr = &gspi_ctrlr, .bus_start = 1,
	  .bus_end =  1 + (CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX - 1)},
#endif
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);

#if ENV_RAMSTAGE

static int spi_dev_to_bus(struct device *dev)
{
	return spi_soc_devfn_to_bus(dev->path.pci.devfn);
}

static struct spi_bus_operations spi_bus_ops = {
	.dev_to_bus			= &spi_dev_to_bus,
};

static struct device_operations spi_dev_ops = {
	.read_resources			= &pci_dev_read_resources,
	.set_resources			= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
	.scan_bus			= &scan_generic_bus,
	.ops_spi_bus			= &spi_bus_ops,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_CNL_HWSEQ_SPI,
	PCI_DEVICE_ID_INTEL_CNL_SPI0,
	PCI_DEVICE_ID_INTEL_CNL_SPI1,
	PCI_DEVICE_ID_INTEL_CNL_SPI2,
	0
};

static const struct pci_driver pch_spi __pci_driver = {
	.ops				= &spi_dev_ops,
	.vendor				= PCI_VENDOR_ID_INTEL,
	.devices			= pci_device_ids,
};
#endif
