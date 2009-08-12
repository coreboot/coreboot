/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "i82801gx.h"

#define HDA_ICII_REG 0x68
#define   HDA_ICII_BUSY (1 << 0)
#define   HDA_ICII_VALID  (1 << 1)

typedef struct southbridge_intel_i82801gx_config config_t;

static int set_bits(u8 * port, u32 mask, u32 val)
{
	u32 reg32;
	int count;

	/* Write (val & mask) to port */
	val &= mask;
	reg32 = readl(port);
	reg32 &= ~mask;
	reg32 |= val;
	writel(reg32, port);

	/* Wait for readback of register to
	 * match what was just written to it
	 */
	count = 50;
	do {
		/* Wait 1ms based on BKDG wait time */
		mdelay(1);
		reg32 = readl(port);
		reg32 &= mask;
	} while ((reg32 != val) && --count);

	/* Timeout occured */
	if (!count)
		return -1;
	return 0;
}

static int codec_detect(u8 * base)
{
	u32 reg32;

	/* Set Bit0 to 0 to enter reset state (BAR + 0x8)[0] */
	if (set_bits(base + 0x08, 1, 0) == -1)
		goto no_codec;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + 0x08, 1, 1) == -1)
		goto no_codec;

	/* Read in Codec location (BAR + 0xe)[2..0]*/
	reg32 = readl(base + 0xe);
	reg32 &= 0x0f;
	if (!reg32)
		goto no_codec;

	return reg32;

no_codec:
	/* Codec Not found */
	/* Put HDA back in reset (BAR + 0x8) [0] */
	set_bits(base + 0x08, 1, 0);
	printk_debug("Azalia: No codec!\n");
	return 0;
}

static u32 cim_verb_data[] = {
	0x00172000,
	0x00172100,
	0x001722EC,
	0x00172310,

	/* Pin Complex (NID 0x12) */
	0x01271CF0,
	0x01271D11,
	0x01271E11,
	0x01271F41,
	/* Pin Complex (NID 0x14) */
	0x01471C10,
	0x01471D01,
	0x01471E13,
	0x01471F99,
	/* Pin Complex (NID 0x15) */
	0x01571C20,
	0x01571D40,
	0x01571E21,
	0x01571F01,
	/* Pin Complex (NID 0x16) */
	0x01671CF0,
	0x01671D11,
	0x01671E11,
	0x01671F41,
	/* Pin Complex (NID 0x18) */
	0x01871C30,
	0x01871D98,
	0x01871EA1,
	0x01871F01,
	/* Pin Complex (NID 0x19) */
	0x01971C31,
	0x01971D09,
	0x01971EA3,
	0x01971F99,
	/* Pin Complex (NID 0x1A) */
	0x01A71C3F,
	0x01A71D98,
	0x01A71EA1,
	0x01A71F02,
	/* Pin Complex (NID 0x1B) */
	0x01B71C1F,
	0x01B71D40,
	0x01B71E21,
	0x01B71F02,
	/* Pin Complex (NID 0x1C) */
	0x01C71CF0,
	0x01C71D11,
	0x01C71E11,
	0x01C71F41,
	/* Pin Complex (NID 0x1D) */
	0x01D71CF0,
	0x01D71D11,
	0x01D71E11,
	0x01D71F41,
	/* Pin Complex (NID 0x1E) */
	0x01E71CF0,
	0x01E71D11,
	0x01E71E11,
	0x01E71F41,
	/* Pin Complex (NID 0x1F) */
	0x01F71CF0,
	0x01F71D11,
	0x01F71E11,
	0x01F71F41,
};

static unsigned find_verb(struct device *dev, u32 viddid, u32 ** verb)
{
	config_t *config = dev->chip_info;

	if (config == NULL) {
		printk_err("\ni82801gx_azalia: Not mentioned in mainboard's Config.lb!\n");
		return 0;
	}

	printk_debug("Azalia: dev=%s\n", dev_path(dev));
	printk_debug("Azalia: Default viddid=%x\n", (u32)config->hda_viddid);
	printk_debug("Azalia: Reading viddid=%x\n", viddid);

	if (viddid != config->hda_viddid)
		return 0;

	*verb = (u32 *) cim_verb_data;

	return sizeof(cim_verb_data) / sizeof(u32);
}

/**
 *  Wait 50usec for for the codec to indicate it is ready
 *  no response would imply that the codec is non-operative
 */

static int wait_for_ready(u8 *base)
{
	/* Use a 50 usec timeout - the Linux kernel uses the
	 * same duration */

	int timeout = 50;

	while(timeout--) {
		u32 reg32 = readl(base +  HDA_ICII_REG);
		if (!(reg32 & HDA_ICII_BUSY))
			return 0;
		udelay(1);
	}

	return -1;
}

/**
 *  Wait 50usec for for the codec to indicate that it accepted
 *  the previous command.  No response would imply that the code
 *  is non-operative
 */

static int wait_for_valid(u8 *base)
{
	/* Use a 50 usec timeout - the Linux kernel uses the
	 * same duration */

	int timeout = 50;
	while(timeout--) {
		u32 reg32 = readl(base + HDA_ICII_REG);
		if ((reg32 & (HDA_ICII_VALID | HDA_ICII_BUSY)) ==
			HDA_ICII_VALID)
			return 0;
		udelay(1);
	}

	return 1;
}

static void codec_init(struct device *dev, u8 * base, int addr)
{
	u32 reg32;
	u32 *verb;
	u32 verb_size;
	int i;

	/* 1 */
	if (wait_for_ready(base) == -1)
		return;

	reg32 = (addr << 28) | 0x000f0000;
	writel(reg32, base + 0x60);

	if (wait_for_valid(base) == -1)
		return;

	reg32 = readl(base + 0x64);

	/* 2 */
	printk_debug("Azalia: codec viddid: %08x\n", reg32);
	verb_size = find_verb(dev, reg32, &verb);

	if (!verb_size) {
		printk_debug("Azalia: No verb!\n");
		return;
	}

	printk_debug("Azalia: verb_size: %d\n", verb_size);
	/* 3 */
	for (i = 0; i < verb_size; i++) {
		if (wait_for_ready(base) == -1)
			return;

		writel(verb[i], base + 0x60);

		if (wait_for_valid(base) == -1)
			return;
	}
	printk_debug("Azalia: verb loaded.\n");
}

static void codecs_init(struct device *dev, u8 * base, u32 codec_mask)
{
	int i;
	for (i = 2; i >= 0; i--) {
		if (codec_mask & (1 << i))
			codec_init(dev, base, i);
	}
}

static void azalia_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;
	u8 reg8;
	u32 reg32;

#if CONFIG_MMCONF_SUPPORT
	// ESD
	reg32 = pci_mmio_read_config32(dev, 0x134);
	reg32 &= 0xff00ffff;
	reg32 |= (2 << 16);
	pci_mmio_write_config32(dev, 0x134, reg32);

	// Link1 description
	reg32 = pci_mmio_read_config32(dev, 0x140);
	reg32 &= 0xff00ffff;
	reg32 |= (2 << 16);
	pci_mmio_write_config32(dev, 0x140, reg32);

	// Port VC0 Resource Control Register
	reg32 = pci_mmio_read_config32(dev, 0x114);
	reg32 &= 0xffffff00;
	reg32 |= 1;
	pci_mmio_write_config32(dev, 0x114, reg32);

	// VCi traffic class
	reg8 = pci_mmio_read_config8(dev, 0x44);
	reg8 |= (7 << 0); // TC7
	pci_mmio_write_config8(dev, 0x44, reg8);

	// VCi Resource Control
	reg32 = pci_mmio_read_config32(dev, 0x120);
	reg32 |= (1 << 31);
	reg32 |= (1 << 24); // VCi ID
	reg32 |= (0x80 << 0); // VCi map
	pci_mmio_write_config32(dev, 0x120, reg32);
#else
#error ICH7 Azalia required CONFIG_MMCONF_SUPPORT
#endif

	/* Set Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	pci_write_config8(dev, 0x3c, 0x0a); // unused?

	// TODO Actually check if we're AC97 or HDA instead of hardcoding this
	// here, in Config.lb and/or auto.c.
	reg8 = pci_read_config8(dev, 0x40);
	reg8 |= (1 << 3); // Clear Clock Detect Bit
	pci_write_config8(dev, 0x40, reg8);
	reg8 &= ~(1 << 3); // Keep CLKDETCLR from clearing the bit over and over
	pci_write_config8(dev, 0x40, reg8);
	reg8 |= (1 << 2); // Enable clock detection
	pci_write_config8(dev, 0x40, reg8);
	mdelay(1);
	reg8 = pci_read_config8(dev, 0x40);
	printk_debug("Azalia: codec type: %s\n", (reg8 & (1 << 1))?"Azalia":"AC97");

	//
	reg8 = pci_read_config8(dev, 0x40); // Audio Control
	reg8 |= 1; // Select Azalia mode. This needs to be controlled via Config.lb
	pci_write_config8(dev, 0x40, reg8);

	reg8 = pci_read_config8(dev, 0x4d); // Docking Status
	reg8 &= ~(1 << 7); // Docking not supported
	pci_write_config8(dev, 0x4d, reg8);
#if 0
	/* Set routing pin */
	pci_write_config32(dev, 0xf8, 0x0);
	pci_write_config8(dev, 0xfc, 0xAA);

	/* Set INTA */
	pci_write_config8(dev, 0x63, 0x0);

	/* Enable azalia, disable ac97 */
	// pm_iowrite(0x59, 0xB);
#endif

	res = find_resource(dev, 0x10);
	if (!res)
		return;

	// NOTE this will break as soon as the Azalia get's a bar above
	// 4G. Is there anything we can do about it?
	base = (u8 *) ((u32)res->base);
	printk_debug("Azalia: base = %08x\n", (u32)base);
	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk_debug("Azalia: codec_mask = %02x\n", codec_mask);
		codecs_init(dev, base, codec_mask);
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
	.enable			= i82801gx_enable,
	.ops_pci		= &azalia_pci_ops,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_azalia __pci_driver = {
	.ops	= &azalia_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d8,
};

