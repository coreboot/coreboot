/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
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

/* This code should work for all ICH* southbridges with a NIC. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static struct device_operations nic_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= 0,
	.scan_bus		= 0,
};

/* Note: There's no NIC on 82801AA/AB (ICH/ICH0). */

/* 82801BA/BAM/CA/CAM (ICH2/ICH2-M/ICH3-S/ICH3-M) */
static const struct pci_driver i82801ba_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801BA_LAN,
};

/* 82801DB/DBL/DBM (ICH4/ICH4-L/ICH4-M) */
static const struct pci_driver i82801db_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801DB_LAN,
};

/* 82801EB/ER (ICH5/ICH5R) */
static const struct pci_driver i82801eb_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801EB_LAN,
};

/* 82801FB/FR/FW/FRW/FBM (ICH6/ICH6R/ICH6W/ICH6RW/ICH6-M) */
static const struct pci_driver i82801fb_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801FB_LAN,
};

/* 82801E (C-ICH) */
static const struct pci_driver i82801e_nic1 __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801E_LAN1,
};

static const struct pci_driver i82801e_nic2 __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801E_LAN2,
};

