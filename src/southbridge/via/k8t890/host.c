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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "k8x8xx.h"

static void host_enable(struct device *dev)
{
	/* Multiple function control */
	pci_write_config8(dev, K8T890_MULTIPLE_FN_EN, 0x01);

	print_debug(" VIA_X_0 device dump:\n");
	dump_south(dev);
}


static void host_init(struct device *dev)
{
	u8 reg;

	/* AGP Capability Header Control */
	reg = pci_read_config8(dev, 0x4d);
	reg |= 0x20; /* GART access enabled by either D0F0 Rx90[8] or D1F0 Rx90[8] */
	pci_write_config8(dev, 0x4d, reg);

	/* GD Output Stagger Delay */
	reg = pci_read_config8(dev, 0x42);
	reg |= 0x10; /* AD[31:16] with 1ns */
	pci_write_config8(dev, 0x42, reg);

	/* AGP Control */
	reg = pci_read_config8(dev, 0xbc);
	reg |= 0x20; /* AGP Read Snoop DRAM Post-Write Buffer */
	pci_write_config8(dev, 0xbc, reg);

}

static const struct device_operations host_ops_t = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= host_enable,
	.ops_pci		= 0,
};

static const struct device_operations host_ops_m = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= host_enable,
	.init			= host_init,
	.ops_pci		= 0,
};

static const struct pci_driver northbridge_driver_t800 __pci_driver = {
	.ops	= &host_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T800_HT_AGP_CTR,
};

static const struct pci_driver northbridge_driver_m800 __pci_driver = {
	.ops	= &host_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M800_HT_AGP_CTR,
};

static const struct pci_driver northbridge_driver_t890 __pci_driver = {
	.ops	= &host_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_0,
};

static const struct pci_driver northbridge_driver_t890cf __pci_driver = {
	.ops	= &host_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CF_0,
};

static const struct pci_driver northbridge_driver_m890 __pci_driver = {
	.ops	= &host_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M890CE_0,
};
