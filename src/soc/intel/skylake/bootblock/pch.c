/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 * Copyright (C) 2016 Intel Corporation.
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
#include <soc/bootblock.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr.h>
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

static void enable_p2sbbar(void)
{
	device_t dev = PCH_DEV_P2SB;

	/* Enable PCR Base address in PCH */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, PCH_PCR_BASE_ADDRESS);

	/* Enable P2SB MSE */
	pci_write_config8(dev, PCI_COMMAND,
			  PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/*
	 * Enable decoding for HPET memory address range.
	 * HPTC_OFFSET(0x60) bit 7, when set the P2SB will decode
	 * the High Performance Timer memory address range
	 * selected by bits 1:0
	 */
	pci_write_config8(dev, HPTC_OFFSET, HPTC_ADDR_ENABLE_BIT);
}

void bootblock_pch_early_init(void)
{
	enable_spibar();
	enable_spi_prefetch();
	enable_p2sbbar();
}
