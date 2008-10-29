/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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
#include "i82801gx.h"

static void ac97_audio_init(struct device *dev)
{
	// XXX init AC97 codecs.
}

static void ac97_modem_init(struct device *dev)
{
	// XXX init modem?
}

static struct device_operations ac97_audio_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ac97_audio_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
};

static struct device_operations ac97_modem_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ac97_modem_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
/* Note: 82801GU (ICH7-U) doesn't have AC97 audio. */
static const struct pci_driver i82801gx_ac97_audio __pci_driver = {
	.ops	= &ac97_audio_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801GB_AC97_AUDIO,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
/* Note: 82801GU (ICH7-U) doesn't have AC97 modem. */
static const struct pci_driver i82801gx_ac97_modem __pci_driver = {
	.ops	= &ac97_modem_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801GB_AC97_MODEM,
};
