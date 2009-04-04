/*
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * Enable IDE controller of the W83C553F chip.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "w83c553f.h"

#ifndef CONFIG_IDE_MAXBUS
#define CONFIG_IDE_MAXBUS	2
#endif
#ifndef CONFIG_IDE_MAXDEVICE
#define CONFIG_IDE_MAXDEVICE	(CONFIG_IDE_MAXBUS*2)
#endif

uint32_t ide_base[CONFIG_IDE_MAXBUS]; 

static void 
w83c553_ide_init(struct device *dev)
{
	unsigned char reg8;
	unsigned short reg16;
	unsigned int reg32;

	printk_info("Configure W83C553F IDE (Function 1)\n");

	/*
	 * Enable native mode on IDE ports and set base address.
	 */
	reg8 = W83C553F_PIR_P1NL | W83C553F_PIR_P0NL;
	pci_write_config8(dev, W83C553F_PIR, reg8);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, 0x1f0);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0x3f6);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_1);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	pci_write_config32(dev, PCI_BASE_ADDRESS_2, 0x170);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	pci_write_config32(dev, PCI_BASE_ADDRESS_3, 0xffffffff);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_3);
	pci_write_config32(dev, PCI_BASE_ADDRESS_3, 0x376);
	reg32 = pci_read_config32(dev, PCI_BASE_ADDRESS_3);

	/*
	 * Set read-ahead duration to 0xff
	 * Enable P0 and P1
	 */
	reg32 = 0x00ff0000 | W83C553F_IDECSR_P1EN | W83C553F_IDECSR_P0EN;
	pci_write_config32(dev, W83C553F_IDECSR, reg32);

	ide_base[0] = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	printk_debug("ide bus offset = 0x%x\n", ide_base[0]);
	ide_base[0] &= ~1;
#if CONFIG_IDE_MAXBUS > 1
	ide_base[1] = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	ide_base[1] &= ~1;
#endif

	/*
	 * Enable function 1, IDE -> busmastering and IO space access
	 */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MASTER | PCI_COMMAND_IO;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	printk_info("IDE configuration complete\n");
}

static struct device_operations w83c553_ide_ops  = {
        .read_resources   = pci_dev_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init             = w83c553_ide_init,
        .scan_bus         = 0,
};

static const struct pci_driver w83c553f_ide_pci_driver __pci_driver = {
	/* w83c553f_ide */
	.ops = &w83c553_ide_ops,
	.device = PCI_DEVICE_ID_WINBOND_82C105,
	.vendor = PCI_VENDOR_ID_WINBOND,
};
