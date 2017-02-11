/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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
#include <soc/ramstage.h>
#include <spi-generic.h>

/* SPI controller managing the flash-device SPI. */
static int flash_spi_ctrlr_setup(const struct spi_slave *dev)
{
	if ((dev->bus != 0) || (dev->cs != 0)) {
		printk(BIOS_ERR, "%s: Unsupported device bus=0x%x,cs=0x%x!\n",
			__func__, dev->bus, dev->cs);
		return -1;
	}

	return 0;
}

static const struct spi_ctrlr flash_spi_ctrlr = {
	.setup = flash_spi_ctrlr_setup,
};

static int gspi_ctrlr_get_config(const struct spi_slave *dev,
				struct spi_cfg *cfg)
{
	if (dev->cs != 0) {
		printk(BIOS_ERR, "%s: Unsupported device "
		       "bus=0x%x,cs=0x%x!\n", __func__, dev->bus, dev->cs);
		return -1;
	}

	cfg->clk_phase = SPI_CLOCK_PHASE_FIRST;
	cfg->clk_polarity = SPI_POLARITY_LOW;
	cfg->cs_polarity = SPI_POLARITY_LOW;
	cfg->wire_mode = SPI_4_WIRE_MODE;
	cfg->data_bit_length = 8;

	return 0;
}

static const struct spi_ctrlr gspi_ctrlr = {
	.get_config = gspi_ctrlr_get_config,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{ .ctrlr = &flash_spi_ctrlr, .bus_start = 0, .bus_end = 0 },
	{ .ctrlr = &gspi_ctrlr, .bus_start = 1, .bus_end = 2 },
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);

#if ENV_RAMSTAGE && !defined(__SIMPLE_DEVICE__)

static int spi_dev_to_bus(struct device *dev)
{
	return spi_devfn_to_bus(dev->path.pci.devfn);
}

static struct spi_bus_operations spi_bus_ops = {
	.dev_to_bus			= &spi_dev_to_bus,
};

static struct device_operations spi_dev_ops = {
	.read_resources			= &pci_dev_read_resources,
	.set_resources			= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
	.scan_bus			= &scan_generic_bus,
	.ops_pci			= &soc_pci_ops,
	.ops_spi_bus			= &spi_bus_ops,
};

static const unsigned short pci_device_ids[] = {
	0x9d24, 0x9d29, 0x9d2a, 0
};

static const struct pci_driver pch_spi __pci_driver = {
	.ops				= &spi_dev_ops,
	.vendor				= PCI_VENDOR_ID_INTEL,
	.devices			= pci_device_ids,
};
#endif
