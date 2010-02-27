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

/* This code should work for all ICH* southbridges with USB. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801bx.h"

static void usb_init(struct device *dev)
{
	/* TODO: Any init needed? Some ports have it, others don't. */
}

static struct device_operations usb_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_init,
	.scan_bus		= 0,
	.enable			= i82801bx_enable,
};

/* 82801AA (ICH) */
static const struct pci_driver i82801aa_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801AA_USB,
};

/* 82801AB (ICH0) */
static const struct pci_driver i82801ab_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801AB_USB,
};

/* 82801BA/BAM (ICH2/ICH2-M) */
static const struct pci_driver i82801ba_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801BA_USB1,
};

static const struct pci_driver i82801ba_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801BA_USB2,
};

/* 82801CA/CAM (ICH3-S/ICH3-M) */
static const struct pci_driver i82801ca_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801CA_USB1,
};

static const struct pci_driver i82801ca_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801CA_USB2,
};

static const struct pci_driver i82801ca_usb3 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801CA_USB3,
};

/* 82801DB/DBL/DBM (ICH4/ICH4-L/ICH4-M) */
static const struct pci_driver i82801db_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801DB_USB1,
};

static const struct pci_driver i82801db_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801DB_USB2,
};

static const struct pci_driver i82801db_usb3 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801DB_USB3,
};

/* 82801EB/ER (ICH5/ICH5R) */
static const struct pci_driver i82801eb_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801EB_USB1,
};

static const struct pci_driver i82801eb_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801EB_USB2,
};

static const struct pci_driver i82801eb_usb3 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801EB_USB3,
};

static const struct pci_driver i82801eb_usb4 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801EB_USB4,
};

/* 82801FB/FR/FW/FRW/FBM (ICH6/ICH6R/ICH6W/ICH6RW/ICH6-M) */
static const struct pci_driver i82801fb_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801FB_USB1,
};

static const struct pci_driver i82801fb_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801FB_USB2,
};

static const struct pci_driver i82801fb_usb3 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801FB_USB3,
};

static const struct pci_driver i82801fb_usb4 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801FB_USB4,
};

/* 82801E (C-ICH) */
static const struct pci_driver i82801e_usb __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801E_USB,
};
