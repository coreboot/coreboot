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

static int set_bits(u8 * port, u32 mask, u32 val)
{
	u32 dword;
	int count;

	val &= mask;
	dword = readl(port);
	dword &= ~mask;
	dword |= val;
	writel(dword, port);

	count = 50;
	do {
		dword = readl(port);
		dword &= mask;
		udelay(100);
	} while ((dword != val) && --count);

	if (!count)
		return -1;

	udelay(540);
	return 0;
}

static int codec_detect(u8 * base)
{
	u32 dword;

	/* 1 */
	set_bits(base + 0x08, 1, 1);

	/* 2 */
	dword = readl(base + 0x0e);
	dword |= 7;
	writel(dword, base + 0x0e);

	/* 3 */
	set_bits(base + 0x08, 1, 0);

	/* 4 */
	set_bits(base + 0x08, 1, 1);

	/* 5 */
	dword = readl(base + 0xe);
	dword &= 7;

	/* 6 */
	if (!dword) {
		set_bits(base + 0x08, 1, 0);
		printk_debug("No codec!\n");
		return 0;
	}
	return dword;

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

static unsigned find_verb(u32 viddid, u32 ** verb)
{
	device_t azalia_dev = dev_find_slot(0, PCI_DEVFN(0x14, 2));
	struct southbridge_intel_i82801gx_config *cfg =
	    (struct southbridge_intel_i82801gx_config *)azalia_dev->chip_info;
	printk_debug("Dev=%s\n", dev_path(azalia_dev));
	printk_debug("Default viddid=%x\n", cfg->hda_viddid);
	printk_debug("Reading viddid=%x\n", viddid);
	if (!cfg)
		return 0;
	if (viddid != cfg->hda_viddid)
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
		u32 dword=readl(base +  HDA_ICII_REG);
		if (!(dword & HDA_ICII_BUSY))
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
		u32 dword = readl(base + HDA_ICII_REG);
		if ((dword & (HDA_ICII_VALID | HDA_ICII_BUSY)) ==
			HDA_ICII_VALID)
			return 0;
		udelay(1);
	}

	return 1;
}

static void codec_init(u8 * base, int addr)
{
	u32 dword;
	u32 *verb;
	u32 verb_size;
	int i;

	/* 1 */
	if (wait_for_ready(base) == -1)
		return;

	dword = (addr << 28) | 0x000f0000;
	writel(dword, base + 0x60);

	if (wait_for_valid(base) == -1)
		return;

	dword = readl(base + 0x64);

	/* 2 */
	printk_debug("codec viddid: %08x\n", dword);
	verb_size = find_verb(dword, &verb);

	if (!verb_size) {
		printk_debug("No verb!\n");
		return;
	}

	printk_debug("verb_size: %d\n", verb_size);
	/* 3 */
	for (i = 0; i < verb_size; i++) {
		if (wait_for_ready(base) == -1)
			return;

		writel(verb[i], base + 0x60);

		if (wait_for_valid(base) == -1)
			return;
	}
	printk_debug("verb loaded!\n");
}

static void codecs_init(u8 * base, u32 codec_mask)
{
	int i;
	for (i = 2; i >= 0; i--) {
		if (codec_mask & (1 << i))
			codec_init(base, i);
	}
}

static void azalia_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	/* Set routing pin */
	pci_write_config32(dev, 0xf8, 0x0);
	pci_write_config8(dev, 0xfc, 0xAA);

	/* Set INTA */
	pci_write_config8(dev, 0x63, 0x0);

	/* Enable azalia, disable ac97 */
	// pm_iowrite(0x59, 0xB);

	res = find_resource(dev, 0x10);
	if (!res)
		return;

	base = (u8 *) ((u32)res->base);
	printk_debug("base = %08x\n", base);
	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk_debug("codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}
}

static struct device_operations azalia_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= azalia_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_azalia __pci_driver = {
	.ops	= &azalia_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d8,
};

