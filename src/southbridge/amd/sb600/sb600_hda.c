/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"

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
	0x01471c10,
	0x01471d40,
	0x01471e01,
	0x01471f01,
/* 1 */
	0x01571c12,
	0x01571d10,
	0x01571e01,
	0x01571f01,
/* 2 */
	0x01671c11,
	0x01671d60,
	0x01671e01,
	0x01671f01,
/* 3 */
	0x01771c14,
	0x01771d20,
	0x01771e01,
	0x01771f01,
/* 4 */
	0x01871c30,
	0x01871d90,
	0x01871ea1,
	0x01871f01,
/* 5 */
	0x01971cf0,
	0x01971d11,
	0x01971e11,
	0x01971f41,
/* 6 */
	0x01a71c80,
	0x01a71d30,
	0x01a71e81,
	0x01a71f01,
/* 7 */
	0x01b71cf0,
	0x01b71d11,
	0x01b71e11,
	0x01b71f41,
/* 8 */
	0x01c71cf0,
	0x01c71d11,
	0x01c71e11,
	0x01c71f41,
/* 9 */
	0x01d71cf0,
	0x01d71d11,
	0x01d71e11,
	0x01d71f41,
/* 10 */
	0x01e71c50,
	0x01e71d11,
	0x01e71e44,
	0x01e71f01,
/* 11 */
	0x01f71c60,
	0x01f71d61,
	0x01f71ec4,
	0x01f71f01,
};
static unsigned find_verb(u32 viddid, u32 ** verb)
{
	device_t azalia_dev = dev_find_slot(0, PCI_DEVFN(0x14, 2));
	struct southbridge_amd_sb600_config *cfg =
	    (struct southbridge_amd_sb600_config *)azalia_dev->chip_info;
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

static void codec_init(u8 * base, int addr)
{
	u32 dword;
	u32 *verb;
	u32 verb_size;
	int i;

	/* 1 */
	do {
		dword = readl(base + 0x68);
	} while (dword & 1);

	dword = (addr << 28) | 0x000f0000;
	writel(dword, base + 0x60);

	do {
		dword = readl(base + 0x68);
	} while ((dword & 3) != 2);

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
		do {
			dword = readl(base + 0x68);
		} while (dword & 1);

		writel(verb[i], base + 0x60);

		do {
			dword = readl(base + 0x68);
		} while ((dword & 3) != 2);
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

static void hda_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	/* SM Setting */
	device_t hda_dev;
	hda_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	/* Set routing pin */
	pci_write_config32(dev, 0xf8, 0x0);
	pci_write_config8(dev, 0xfc, 0xAA);
	/* Set INTA */
	pci_write_config8(dev, 0x63, 0x0);
	/* Enable azalia, disable ac97 */
	pm_iowrite(0x59, 0xB);

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

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations hda_audio_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	/*.enable           = sb600_enable, */
	.init = hda_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static struct pci_driver hdaaudio_driver __pci_driver = {
	.ops = &hda_audio_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB600_HDA,
};
