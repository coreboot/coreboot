/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

/* This code should work for all ICH* southbridges with a NIC. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void nic_init(struct device *dev)
{
	/* Nothing yet */
}

static struct device_operations nic_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= nic_init,
	.scan_bus		= 0,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
/* Note: 82801GU (ICH7-U) doesn't have a NIC. */
/* PCI ID loaded from EEPROM. If EEPROM is 0, 0x27dc is used. */
static const struct pci_driver i82801gx_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27dc,
};
