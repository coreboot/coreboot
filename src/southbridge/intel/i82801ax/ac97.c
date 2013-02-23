/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Tyan Computer
 * (Written by Yinghai Lu <yinghailu@gmail.com> for Tyan Computer)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801ax.h"

static struct device_operations ac97_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= 0,
	.scan_bus		= 0,
	.enable			= i82801ax_enable,
};

/* 82801AA (ICH) */
static const struct pci_driver i82801aa_ac97_audio __pci_driver = {
	.ops	= &ac97_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801AA_AC97_AUDIO,
};

static const struct pci_driver i82801aa_ac97_modem __pci_driver = {
	.ops	= &ac97_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801AA_AC97_MODEM,
};

/* 82801AB (ICH0) */
static const struct pci_driver i82801ab_ac97_audio __pci_driver = {
	.ops	= &ac97_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801AB_AC97_AUDIO,
};

static const struct pci_driver i82801ab_ac97_modem __pci_driver = {
	.ops	= &ac97_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801AB_AC97_MODEM,
};
