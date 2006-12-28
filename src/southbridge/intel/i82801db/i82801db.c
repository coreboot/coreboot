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
#include "i82801db.h"

void i82801db_enable(device_t dev)
{
	device_t lpc_dev;
	uint16_t word;

	printk_debug("Entering %s\n", __FUNCTION__);
	/* See if we are behind the i82801db pci bridge 
	lpc_dev = dev_find_slot(dev->bus->secondary, PCI_DEVFN(0x1f, 0));
	*/
	lpc_dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	if (!lpc_dev) {
		return;
	}
	
	word = 0x0040;
	pci_write_config16(lpc_dev, 0xf2, word);
	/*
	if((dev->path.u.pci.devfn &0xf8)== 0xf8) {
		index = dev->path.u.pci.devfn & 7;
	}
	else if((dev->path.u.pci.devfn &0xf8)== 0xe8) {
		index = (dev->path.u.pci.devfn & 7) +8;
	}

	reg = reg_old = pci_read_config16(lpc_dev, 0xf2);
	reg &= ~(1 << index);
	if (!dev->enabled) {
		reg |= (1 << index);
	}
	if (reg != reg_old) {
		printk_debug("Trying to enable device, [0:1f:0] reg 0xf2 <- %04x\n", reg);
		pci_write_config16(lpc_dev, 0xf2, reg);
	}
	*/	
}

struct chip_operations southbridge_intel_i82801db_ops = {
	CHIP_NAME("Intel 82801DB Southbridge")
	.enable_dev = i82801db_enable,
};
