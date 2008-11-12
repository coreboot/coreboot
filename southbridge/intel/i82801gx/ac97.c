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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "i82801gx.h"

static void ac97_audio_init(struct device *dev)
{
	// XXX init AC97 codecs.
}

static void ac97_modem_init(struct device *dev)
{
	// XXX init modem?
}
void i82801gx_enable(struct device * dev);
/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
/* Note: 82801GU (ICH7-U) doesn't have AC97 audio. */
struct device_operations ac97audio = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x27de}}},
	.constructor		 = default_device_constructor,
	.phase3_chip_setup_dev	= i82801gx_enable,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = ac97_audio_init,
	.ops_pci		 = &pci_dev_ops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
/* Note: 82801GU (ICH7-U) doesn't have AC97 modem. */
struct device_operations ac97modem = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x27dd}}},
	.constructor		 = default_device_constructor,
	.phase3_chip_setup_dev	= i82801gx_enable,
	.phase3_scan		 = 0,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = ac97_modem_init,
	.ops_pci		 = &pci_dev_ops_pci,
};

