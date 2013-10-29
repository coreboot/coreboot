/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <romstage_handoff.h>

#include <baytrail/iomap.h>
#include <baytrail/lpc.h>
#include <baytrail/nvs.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>
#include <baytrail/ramstage.h>

static inline void
add_mmio_resource(device_t dev, int i, unsigned long addr, unsigned long size)
{
	mmio_resource(dev, i, addr >> 10, size >> 10);
}

static void sc_add_mmio_resources(device_t dev)
{
	add_mmio_resource(dev, PBASE, PMC_BASE_ADDRESS, 1024);
	add_mmio_resource(dev, IOBASE, IO_BASE_ADDRESS, 16 * 1024);
	add_mmio_resource(dev, IBASE, ILB_BASE_ADDRESS, 1024);
	add_mmio_resource(dev, SBASE, SPI_BASE_ADDRESS, 1024);
	add_mmio_resource(dev, MPBASE, MPHY_BASE_ADDRESS, 1024 * 1024);
	add_mmio_resource(dev, PUBASE, PUNIT_BASE_ADDRESS, 2048);
	add_mmio_resource(dev, RCBA, RCBA_BASE_ADDRESS, 1024);
}

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

static inline int io_range_in_default(int base, int size)
{
	/* Does it start above the range? */
	if (base >= LPC_DEFAULT_IO_RANGE_UPPER)
		return 0;

	/* Is it entirely contained? */
	if (base >= LPC_DEFAULT_IO_RANGE_LOWER &&
	    (base + size) < LPC_DEFAULT_IO_RANGE_UPPER)
		return 1;

	/* This will return not in range for partial overlaps. */
	return 0;
}

/*
 * Note: this function assumes there is no overlap with the default LPC device's
 * claimed range: LPC_DEFAULT_IO_RANGE_LOWER -> LPC_DEFAULT_IO_RANGE_UPPER.
 */
static void sc_add_io_resource(device_t dev, int base, int size, int index)
{
	struct resource *res;

	if (io_range_in_default(base, size))
		return;

	res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void sc_add_io_resources(device_t dev)
{
	struct resource *res;

	/* Add the default claimed IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = LPC_DEFAULT_IO_RANGE_LOWER;
	res->size = LPC_DEFAULT_IO_RANGE_UPPER - LPC_DEFAULT_IO_RANGE_LOWER;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* GPIO */
	sc_add_io_resource(dev, GPIO_BASE_ADDRESS, 256, GBASE);

	/* ACPI */
	sc_add_io_resource(dev, ACPI_BASE_ADDRESS, 128, ABASE);
}

static void sc_read_resources(device_t dev)
{
	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	sc_add_mmio_resources(dev);

	/* Add IO resources. */
	sc_add_io_resources(dev);
}

/*
 * Common code for the south cluster devices.
 */

/* Set bit in function disble register to hide this device. */
static void sc_disable_devfn(device_t dev)
{
	const unsigned long func_dis = PMC_BASE_ADDRESS + FUNC_DIS;
	const unsigned long func_dis2 = PMC_BASE_ADDRESS + FUNC_DIS2;
	uint32_t mask = 0;
	uint32_t mask2 = 0;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(SDIO_DEV, SDIO_FUNC):
		mask |= SDIO_DIS;
		break;
	case PCI_DEVFN(SD_DEV, SD_FUNC):
		mask |= SD_DIS;
		break;
	case PCI_DEVFN(SATA_DEV, SATA_FUNC):
		mask |= SATA_DIS;
		break;
	case PCI_DEVFN(XHCI_DEV, XHCI_FUN):
		mask |= XHCI_DIS;
		/* Disable super speed PHY when XHCI is not available. */
		mask2 |= USH_SS_PHY_DIS:
		break;
	case PCI_DEVFN(LPE_DEV, LPE_FUNC):
		mask |= LPE_DIS;
		break;
	case PCI_DEVFN(MMC_DEV, MMC_FUNC):
		mask |= MMC_DIS;
		break;
	case PCI_DEVFN(SIO_DMA1_DEV, SIO_DMA1_FUNC):
		mask |= SIO_DMA1_DIS;
		break;
	case PCI_DEVFN(I2C1_DEV, I2C1_FUNC):
		mask |= I2C1_DIS;
		break;
	case PCI_DEVFN(I2C2_DEV, I2C2_FUNC):
		mask |= I2C1_DIS;
		break;
	case PCI_DEVFN(I2C3_DEV, I2C3_FUNC):
		mask |= I2C3_DIS;
		break;
	case PCI_DEVFN(I2C4_DEV, I2C4_FUNC):
		mask |= I2C4_DIS;
		break;
	case PCI_DEVFN(I2C5_DEV, I2C5_FUNC):
		mask |= I2C5_DIS;
		break;
	case PCI_DEVFN(I2C6_DEV, I2C6_FUNC):
		mask |= I2C6_DIS;
		break;
	case PCI_DEVFN(I2C7_DEV, I2C7_FUNC):
		mask |= I2C7_DIS;
		break;
	case PCI_DEVFN(TXE_DEV, TXE_FUNC):
		mask |= TXE_DIS;
		break;
	case PCI_DEVFN(HDA_DEV, HDA_FUNC):
		mask |= HDA_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT1_DEV, PCIE_PORT1_FUNC):
		mask |= PCIE_PORT1_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT2_DEV, PCIE_PORT2_FUNC):
		mask |= PCIE_PORT2_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT3_DEV, PCIE_PORT3_FUNC):
		mask |= PCIE_PORT3_DIS;
		break;
	case PCI_DEVFN(PCIE_PORT4_DEV, PCIE_PORT4_FUNC):
		mask |= PCIE_PORT4_DIS;
		break;
	case PCI_DEVFN(EHCI_DEV, EHCI_FUNC):
		mask |= EHCI_DIS;
		break;
	case PCI_DEVFN(SIO_DMA2_DEV, SIO_DMA2_FUNC):
		mask |= SIO_DMA2_DIS;
		break;
	case PCI_DEVFN(PWM1_DEV, PM1_FUNC):
		mask |= PWM1_DIS;
		break;
	case PCI_DEVFN(PWM2_DEV, PWM2_FUNC):
		mask |= PWM2_DIS;
		break;
	case PCI_DEVFN(HSUART1_DEV, HSUART1_FUNC):
		mask |= HSUART1_DIS;
		break;
	case PCI_DEVFN(HSUART2_DEV, HSUART2_FUNC):
		mask |= HSUART2_DIS;
		break;
	case PCI_DEVFN(SPI_DEV, SPI_FUNC):
		mask |= SPI_DIS;
		break;
	case PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC):
		mask2 |= SMBUS_DIS;
		break;
	}

	if (mask != 0) {
		write32(func_dis, read32(func_dis) | mask);
		/* Ensure posted write hits. */
		read32(func_dis);
	}

	if (mask2 != 0) {
		write32(func_dis2, read32(func_dis2) | mask2);
		/* Ensure posted write hits. */
		read32(func_dis2);
	}
}

static inline void set_d3hot_bits(device_t dev, int offset)
{
	uint32_t reg8;
	printk(BIOS_DEBUG, "Power management CAP offset 0x%x.\n", offset);
	reg8 = pci_read_config8(dev, offset + 4);
	reg8 |= 0x3;
	pci_write_config8(dev, offset + 4, reg8);
}

static int place_device_in_d3hot(device_t dev)
{
	unsigned offset;

	offset = pci_find_capability(dev, PCI_CAP_ID_PM);

	if (offset != 0) {
		set_d3hot_bits(dev, offset);
		return 0;
	}

	/* For some reason some of the devices don't have the capability
	 * pointer set correctly. Work around this by hard coding the offset. */
	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(SDIO_DEV, SDIO_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SD_DEV, SD_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(MMC_DEV, MMC_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(LPE_DEV, LPE_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SIO_DMA1_DEV, SIO_DMA1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C1_DEV, I2C1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C2_DEV, I2C2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C3_DEV, I2C3_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C4_DEV, I2C4_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C5_DEV, I2C5_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C6_DEV, I2C6_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(I2C7_DEV, I2C7_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SIO_DMA2_DEV, SIO_DMA2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(PWM1_DEV, PWM1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(PWM2_DEV, PWM2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(HSUART1_DEV, HSUART1_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(HSUART2_DEV, HSUART2_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SPI_DEV, SPI_FUNC):
		offset = 0x80;
		break;
	case PCI_DEVFN(SATA_DEV, SATA_FUNC):
		offset = 0x70;
		break;
	case PCI_DEVFN(XHCI_DEV, XHCI_FUNC):
		offset = 0x70;
		break;
	case PCI_DEVFN(EHCI_DEV, EHCI_FUNC):
		offset = 0x70;
		break;
	case PCI_DEVFN(HDA_DEV, HDA_FUNC):
		offset = 0x50;
		break;
	case PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC):
		offset = 0x50;
		break;
	case PCI_DEVFN(TXE_DEV, TXE_FUNC):
		break;
	case PCI_DEVFN(PCIE_PORT1_DEV, PCIE_PORT1_FUNC):
		offset = 0xa0;
		break;
	case PCI_DEVFN(PCIE_PORT2_DEV, PCIE_PORT2_FUNC):
		offset = 0xa0;
		break;
	case PCI_DEVFN(PCIE_PORT3_DEV, PCIE_PORT3_FUNC):
		offset = 0xa0;
		break;
	case PCI_DEVFN(PCIE_PORT4_DEV, PCIE_PORT4_FUNC):
		offset = 0xa0;
		break;
	}

	if (offset != 0) {
		set_d3hot_bits(dev, offset);
		return 0;
	}

	return -1;
}

/* Common PCI device function disable. */
void southcluster_enable_dev(device_t dev)
{
	uint32_t reg32;

	if (!dev->enabled) {
		int slot = PCI_SLOT(dev->path.pci.devfn);
		int func = PCI_FUNC(dev->path.pci.devfn);
		printk(BIOS_DEBUG, "%s: Disabling device: %02x.%01x\n",
		       dev_path(dev), slot, func);

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Place device in D3Hot */
		if (place_device_in_d3hot(dev) < 0) {
			printk(BIOS_WARNING,
			       "Could not place %02x.%01x into D3Hot. "
			       "Keeping device visible.\n", slot, func);
			return;
		}
		/* Disable this device if possible */
		sc_disable_devfn(dev);
	} else {
		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

static struct device_operations device_ops = {
	.read_resources		= sc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.enable			= southcluster_enable_dev,
	.scan_bus		= NULL,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.device		= LPC_DEVID,
};
