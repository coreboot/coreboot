/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2011 Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "k8x8xx.h"

static void error_enable(struct device *dev)
{
	print_debug(" K8x8xx: Enabling NB error reporting: ");
	/*
	 * bit0 - Enable V-link parity error reporting in 0x50 bit0 (RWC)
	 * bit6 - Parity Error/SERR# Report Through V-Link to SB
	 * bit7 - Parity Error/SERR# Report Through NMI
	 */
	pci_write_config8(dev, 0x58, 0x81);

	print_debug("Done\n");
	/* TODO: enable AGP errors reporting on K8M890 */

	print_debug(" VIA_X_1 device dump:\n");
	dump_south(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static const struct device_operations error_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= error_enable,
	.ops_pci		= &lops_pci,
};

static const unsigned short error_ids[] = {
	PCI_DEVICE_ID_VIA_K8T800_ERR,
	PCI_DEVICE_ID_VIA_K8M800_ERR,
	PCI_DEVICE_ID_VIA_K8T890CE_1,
	PCI_DEVICE_ID_VIA_K8T890CF_1,
	PCI_DEVICE_ID_VIA_K8M890CE_1,
	0
};

static const struct pci_driver northbridge_driver_t800 __pci_driver = {
	.ops	= &error_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.devices= error_ids,
};
