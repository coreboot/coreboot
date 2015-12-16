/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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
#include <arch/io.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/spi.h>

/*
 * Enable Prefetching and Caching.
 */
static void enable_spi_prefetch(void)
{
	u8 reg8 = pci_read_config8(PCH_DEV_SPI, 0xdc);
	reg8 &= ~(3 << 2);
	reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
	pci_write_config8(PCH_DEV_SPI, 0xdc, reg8);
}

static void enable_spibar(void)
{
	device_t dev = PCH_DEV_SPI;
	u8 pcireg;

	/* Assign Resources to SPI Controller */
	/* Clear BIT 1-2 SPI Command Register */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config8(dev, PCI_COMMAND, pcireg);

	/* Program Temporary BAR for SPI */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0,
		SPI_BASE_ADDRESS | PCI_BASE_ADDRESS_SPACE_MEMORY);

	/* Enable Bus Master and MMIO Space */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config8(dev, PCI_COMMAND, pcireg);
}

static void bootblock_southbridge_init(void)
{
	enable_spibar();
	enable_spi_prefetch();
}
