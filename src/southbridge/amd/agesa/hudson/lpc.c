/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include "hudson.h"

static void lpc_init(device_t dev)
{
	u8 byte;
	u32 dword;
	device_t sm_dev;

	/* Enable the LPC Controller */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	dword = pci_read_config32(sm_dev, 0x64);
	dword |= 1 << 20;
	pci_write_config32(sm_dev, 0x64, dword);

	/* Initialize isa dma */
	isa_dma_init();

	/* Enable DMA transaction on the LPC bus */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x40, byte);

	/* Disable the timeout mechanism on LPC */
	byte = pci_read_config8(dev, 0x48);
	byte &= ~(1 << 7);
	pci_write_config8(dev, 0x48, byte);

	/* Disable LPC MSI Capability */
	byte = pci_read_config8(dev, 0x78);
	byte &= ~(1 << 1);
	byte &= ~(1 << 0);	/* Keep the old way. i.e., when bus master/DMA cycle is going
				   on on LPC, it holds PCI grant, so no LPC slave cycle can
				   interrupt and visit LPC. */
	pci_write_config8(dev, 0x78, byte);

	/* bit0: Enable prefetch a cacheline (64 bytes) when Host reads code from SPI rom */
	/* bit3: Fix SPI_CS# timing issue when running at 66M. TODO:A12. */
	byte = pci_read_config8(dev, 0xBB);
	byte |= 1 << 0 | 1 << 3;
	pci_write_config8(dev, 0xBB, byte);

	rtc_check_update_cmos_date(RTC_HAS_ALTCENTURY);

	/* Initialize the real time clock.
	 * The 0 argument tells rtc_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells rtc_init to always initialize the CMOS.
	 */
	rtc_init(0);
}

static void hudson_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);	/* We got one for APIC, or one more for TRAP */

	pci_get_resource(dev, 0xA0); /* SPI ROM base address */

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	//res = new_resource(dev, 3); /* IOAPIC */
	//res->base = 0xfec00000;
	//res->size = 0x00001000;
	//res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	compact_resources(dev);
}

static void hudson_lpc_set_resources(struct device *dev)
{
	struct resource *res;

	/* Special case. SPI Base Address. The SpiRomEnable should STAY set. */
	res = find_resource(dev, SPIROM_BASE_ADDRESS_REGISTER);
	res->base |= PCI_COMMAND_MEMORY;

	pci_dev_set_resources(dev);


}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whos children's resources are to be enabled
 *
 */
static void hudson_lpc_enable_childrens_resources(device_t dev)
{
	printk(BIOS_DEBUG, "hudson_lpc_enable_childrens_resources\n");

}

static void hudson_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	hudson_lpc_enable_childrens_resources(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations lpc_ops = {
	.read_resources = hudson_lpc_read_resources,
	.set_resources = hudson_lpc_set_resources,
	.enable_resources = hudson_lpc_enable_resources,
	.init = lpc_init,
	.scan_bus = scan_static_bus,
	.ops_pci = &lops_pci,
};
static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_ATI_SB900_LPC,
};
