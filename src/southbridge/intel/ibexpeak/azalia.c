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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include <device/azalia_device.h>
#include <southbridge/intel/common/rcba.h>
#include "pch.h"

#define HDA_ICII_REG 0x68
#define HDA_ICII_BUSY (1 << 0)
#define HDA_ICII_VALID (1 << 1)

static int set_bits(void *port, u32 mask, u32 val)
{
	u32 reg32;
	int count;

	/* Write (val & mask) to port */
	val &= mask;
	reg32 = read32(port);
	reg32 &= ~mask;
	reg32 |= val;
	write32(port, reg32);

	/* Wait for readback of register to
	 * match what was just written to it
	 */
	count = 50;
	do {
		/* Wait 1ms based on BKDG wait time */
		mdelay(1);
		reg32 = read32(port);
		reg32 &= mask;
	} while ((reg32 != val) && --count);

	/* Timeout occurred */
	if (!count)
		return -1;
	return 0;
}

static int codec_detect(u8 *base)
{
	u8 reg8;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + 0x08, 1, 1) == -1)
		goto no_codec;

	/* Write back the value once reset bit is set. */
	write16(base + 0x0,
		read16(base + 0x0));

	/* Read in Codec location (BAR + 0xe)[2..0]*/
	reg8 = read8(base + 0xe);
	reg8 &= 0x0f;
	if (!reg8)
		goto no_codec;

	return reg8;

no_codec:
	/* Codec Not found */
	/* Put HDA back in reset (BAR + 0x8) [0] */
	set_bits(base + 0x08, 1, 0);
	printk(BIOS_DEBUG, "Azalia: No codec!\n");
	return 0;
}

static u32 find_verb(struct device *dev, u32 viddid, const u32 ** verb)
{
	int idx=0;

	while (idx < (cim_verb_data_size / sizeof(u32))) {
		u32 verb_size = 4 * cim_verb_data[idx+2]; // in u32
		if (cim_verb_data[idx] != viddid) {
			idx += verb_size + 3; // skip verb + header
			continue;
		}
		*verb = &cim_verb_data[idx+3];
		return verb_size;
	}

	/* Not all codecs need to load another verb */
	return 0;
}

/**
 *  Wait 50usec for the codec to indicate it is ready
 *  no response would imply that the codec is non-operative
 */

static int wait_for_ready(u8 *base)
{
	/* Use a 1msec timeout */

	int timeout = 1000;

	while (timeout--) {
		u32 reg32 = read32(base + HDA_ICII_REG);
		if (!(reg32 & HDA_ICII_BUSY))
			return 0;
		udelay(1);
	}

	return -1;
}

/**
 *  Wait 50usec for the codec to indicate that it accepted
 *  the previous command.  No response would imply that the code
 *  is non-operative
 */

static int wait_for_valid(u8 *base)
{
	u32 reg32;

	/* Send the verb to the codec */
	reg32 = read32(base + HDA_ICII_REG);
	reg32 |= HDA_ICII_BUSY | HDA_ICII_VALID;
	write32(base + HDA_ICII_REG, reg32);

	/* Use a 1msec timeout */

	int timeout = 1000;
	while (timeout--) {
		reg32 = read32(base + HDA_ICII_REG);
		if ((reg32 & (HDA_ICII_VALID | HDA_ICII_BUSY)) ==
			HDA_ICII_VALID)
			return 0;
		udelay(1);
	}

	return -1;
}

static void codec_init(struct device *dev, u8 *base, int addr)
{
	u32 reg32;
	const u32 *verb;
	u32 verb_size;
	int i;

	printk(BIOS_DEBUG, "Azalia: Initializing codec #%d\n", addr);

	/* 1 */
	if (wait_for_ready(base) == -1) {
		printk(BIOS_DEBUG, "  codec not ready.\n");
		return;
	}

	reg32 = (addr << 28) | 0x000f0000;
	write32(base + 0x60, reg32);

	if (wait_for_valid(base) == -1) {
		printk(BIOS_DEBUG, "  codec not valid.\n");
		return;
	}

	reg32 = read32(base + 0x64);

	/* 2 */
	printk(BIOS_DEBUG, "Azalia: codec viddid: %08x\n", reg32);
	verb_size = find_verb(dev, reg32, &verb);

	if (!verb_size) {
		printk(BIOS_DEBUG, "Azalia: No verb!\n");
		return;
	}
	printk(BIOS_DEBUG, "Azalia: verb_size: %d\n", verb_size);

	/* 3 */
	for (i = 0; i < verb_size; i++) {
		if (wait_for_ready(base) == -1)
			return;

		write32(base + 0x60, verb[i]);

		if (wait_for_valid(base) == -1)
			return;
	}
	printk(BIOS_DEBUG, "Azalia: verb loaded.\n");
}

static void codecs_init(struct device *dev, u8 *base, u32 codec_mask)
{
	int i;
	for (i = 3; i >= 0; i--) {
		if (codec_mask & (1 << i))
			codec_init(dev, base, i);
	}

	for (i = 0; i < pc_beep_verbs_size; i++) {
		if (wait_for_ready(base) == -1)
			return;

		write32(base + 0x60, pc_beep_verbs[i]);

		if (wait_for_valid(base) == -1)
			return;
	}
}

static void azalia_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;
	u8 reg8;
	u16 reg16;
	u32 reg32;

	/* Find base address */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	// NOTE this will break as soon as the Azalia get's a bar above
	// 4G. Is there anything we can do about it?
	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "Azalia: base = %08x\n", (u32)base);

	if (RCBA32(0x2030) & (1 << 31)) {
		reg32 = pci_read_config32(dev, 0x120);
		reg32 &= 0xf8ffff01;
		reg32 |= (1 << 24); // 2 << 24 for server
		reg32 |= RCBA32(0x2030) & 0xfe;
		pci_write_config32(dev, 0x120, reg32);

		reg16 = pci_read_config16(dev, 0x78);
		reg16 |= (1 << 11);
		pci_write_config16(dev, 0x78, reg16);
	} else
		printk(BIOS_DEBUG, "Azalia: V1CTL disabled.\n");

	reg32 = pci_read_config32(dev, 0x114);
	reg32 &= ~0xfe;
	pci_write_config32(dev, 0x114, reg32);

	// Set VCi enable bit
	reg32 = pci_read_config32(dev, 0x120);
	reg32 |= (1 << 31);
	pci_write_config32(dev, 0x120, reg32);

	// Enable HDMI codec:
	reg32 = pci_read_config32(dev, 0xc4);
	reg32 |= (1 << 1);
	pci_write_config32(dev, 0xc4, reg32);

	reg8 = pci_read_config8(dev, 0x43);
	reg8 |= (1 << 6);
	pci_write_config8(dev, 0x43, reg8);

	reg32 = pci_read_config32(dev, 0xd0);
	reg32 &= ~(1 << 31);
	pci_write_config32(dev, 0xd0, reg32);

	/* Set Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	pci_write_config8(dev, 0x3c, 0x0a); // unused?

	/* Codec Initialization Programming Sequence */

	/* Take controller out of reset */
	reg32 = read32(base + 0x08);
	reg32 |= (1 << 0);
	write32(base + 0x08, reg32);
	/* Wait 1ms */
	udelay(1000);

	//
	reg8 = pci_read_config8(dev, 0x40); // Audio Control
	reg8 |= 1; // Select Azalia mode. This needs to be controlled via devicetree.cb
	pci_write_config8(dev, 0x40, reg8);

	reg8 = pci_read_config8(dev, 0x4d); // Docking Status
	reg8 &= ~(1 << 7); // Docking not supported
	pci_write_config8(dev, 0x4d, reg8);

	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "Azalia: codec_mask = %02x\n", codec_mask);
		codecs_init(dev, base, codec_mask);
	}

	/* Enable dynamic clock gating */
	reg8 = pci_read_config8(dev, 0x43);
	reg8 &= ~0x7;
	reg8 |= (1 << 2) | (1 << 0);
	pci_write_config8(dev, 0x43, reg8);
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

static const unsigned short pci_device_ids[] = { 0x1c20, 0x1e20, 0x3b56, 0 };

static const struct pci_driver pch_azalia __pci_driver = {
	.ops	 = &azalia_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
