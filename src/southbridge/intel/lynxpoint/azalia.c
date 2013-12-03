/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "pch.h"
#include "hda_verb.h"

const u32 * cim_verb_data = NULL;
u32 cim_verb_data_size = 0;
const u32 * pc_beep_verbs = NULL;
u32 pc_beep_verbs_size = 0;

static void codecs_init(u32 base, u32 codec_mask)
{
	int i;

	/* Can support up to 4 codecs */
	for (i = 3; i >= 0; i--) {
		if (codec_mask & (1 << i))
			hda_codec_init(base, i,
				       cim_verb_data_size,
				       cim_verb_data);
	}

	if (pc_beep_verbs_size && pc_beep_verbs)
		hda_codec_write(base, pc_beep_verbs_size, pc_beep_verbs);
}

static void azalia_pch_init(struct device *dev, u32 base)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	if (RCBA32(0x2030) & (1 << 31)) {
		reg32 = pci_read_config32(dev, 0x120);
		reg32 &= 0xf8ffff01;
		reg32 |= (1 << 25);
		reg32 |= RCBA32(0x2030) & 0xfe;
		pci_write_config32(dev, 0x120, reg32);

		if (!pch_is_lp()) {
			reg16 = pci_read_config16(dev, 0x78);
			reg16 &= ~(1 << 11);
			pci_write_config16(dev, 0x78, reg16);
		}
	} else
		printk(BIOS_DEBUG, "Azalia: V1CTL disabled.\n");

	reg32 = pci_read_config32(dev, 0x114);
	reg32 &= ~0xfe;
	pci_write_config32(dev, 0x114, reg32);

	// Set VCi enable bit
	if (pci_read_config32(dev, 0x120) & ((1 << 24) |
						(1 << 25) | (1 << 26))) {
		reg32 = pci_read_config32(dev, 0x120);
		if (pch_is_lp())
			reg32 &= ~(1 << 31);
		else
			reg32 |= (1 << 31);
		pci_write_config32(dev, 0x120, reg32);
	}

	reg8 = pci_read_config8(dev, 0x43);
	if (pch_is_lp())
		reg8 &= ~(1 << 6);
	else
		reg8 |= (1 << 4);
	pci_write_config8(dev, 0x43, reg8);

	if (!pch_is_lp()) {
		reg32 = pci_read_config32(dev, 0xc0);
		reg32 |= (1 << 17);
		pci_write_config32(dev, 0xc0, reg32);
	}

	/* Additional programming steps */
	reg32 = pci_read_config32(dev, 0xc4);
	if (pch_is_lp())
		reg32 |= (1 << 24);
	else
		reg32 |= (1 << 14);
	pci_write_config32(dev, 0xc4, reg32);

	if (!pch_is_lp()) {
		reg32 = pci_read_config32(dev, 0xd0);
		reg32 &= ~(1 << 31);
		pci_write_config32(dev, 0xd0, reg32);
	}

	reg8 = pci_read_config8(dev, 0x40); // Audio Control
	reg8 |= 1; // Select Azalia mode
	pci_write_config8(dev, 0x40, reg8);

	reg8 = pci_read_config8(dev, 0x4d); // Docking Status
	reg8 &= ~(1 << 7); // Docking not supported
	pci_write_config8(dev, 0x4d, reg8);

	if (pch_is_lp()) {
		reg16 = read32(base + 0x0012);
		reg16 |= (1 << 0);
		write32(base + 0x0012, reg16);

		/* disable Auto Voltage Detector */
		reg8 = pci_read_config8(dev, 0x42);
		reg8 |= (1 << 2);
		pci_write_config8(dev, 0x42, reg8);
	}
}

static void azalia_init(struct device *dev)
{
	u32 base;
	struct resource *res;
	u32 codec_mask;
	u32 reg32;

	/* Find base address */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = (u32)res->base;
	printk(BIOS_DEBUG, "Azalia: base = %08x\n", (u32)base);

	/* Set Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	azalia_pch_init(dev, base);

	codec_mask = hda_codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "Azalia: codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}
}

static void azalia_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations azalia_pci_ops = {
	.set_subsystem    = azalia_set_subsystem,
};

static struct device_operations azalia_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= azalia_init,
	.scan_bus		= 0,
	.ops_pci		= &azalia_pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x8c20, 0x9c20, 0 };

static const struct pci_driver pch_azalia __pci_driver = {
	.ops	 = &azalia_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};

