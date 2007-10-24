/*
 * This file is part of the LinuxBIOS project.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801db.h"

static void ide_init(struct device *dev)
{
	/* Enable IDE devices and timmings */
	uint8_t byte;

	pci_write_config16(dev, 0x40, 0x8000);
	/*
	pci_write_config16(dev, 0x40, 0xa307);
	pci_write_config16(dev, 0x42, 0xa307);
	pci_write_config8(dev, 0x48, 0x05);
	pci_write_config16(dev, 0x4a, 0x0101);
	pci_write_config16(dev, 0x54, 0x5055);
	*/
	
	/*
	byte = pci_read_config8(dev, 0x09);
	byte |= 0x0f;
	pci_write_config8(dev, 0x09, byte);
	*/
	/* force irq into compat mode
	pci_write_config32(dev, 0x10, 0x0);
	pci_write_config32(dev, 0x14, 0x0);
	pci_write_config32(dev, 0x18, 0x0);
	pci_write_config32(dev, 0x1c, 0x0);
	

	pci_write_config8(dev, 0x3d, 00);
	*/
	pci_write_config8(dev, 0x3c, 0xff);
	


	printk_debug("IDE Enabled\n");
}

static struct device_operations ide_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.enable		  = 0,
	.init             = ide_init,
	.scan_bus         = 0,
};

static const struct pci_driver ide_driver __pci_driver = {
	.ops    = &ide_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DB_IDE,
};

