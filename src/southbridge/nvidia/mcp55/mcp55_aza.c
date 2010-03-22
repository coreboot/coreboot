/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "mcp55.h"

static int set_bits(uint8_t *port, uint32_t mask, uint32_t val)
{
	uint32_t dword;
	int count;

	val &= mask;
	dword = read32(port);
	dword &= ~mask;
	dword |= val;
	write32(port, dword);

	count = 50;
	do {
		dword = read32(port);
		dword &= mask;
		udelay(100);
	} while ((dword != val) && --count);

	if(!count) return -1;

	udelay(540);
	return 0;

}

static int codec_detect(uint8_t *base)
{
	uint32_t dword;

	/* 1 */
	set_bits(base + 0x08, 1, 1);

	/* 2 */
	dword = read32(base + 0x0e);
	dword |= 7;
	write32(base + 0x0e, dword);

	/* 3 */
	set_bits(base + 0x08, 1, 0);

	/* 4 */
	set_bits(base + 0x08, 1, 1);

	/* 5 */
	dword = read32(base + 0xe);
	dword &= 7;

	/* 6 */
	if(!dword) {
		set_bits(base + 0x08, 1, 0);
		printk(BIOS_DEBUG, "No codec!\n");
		return 0;
	}
	return dword;

}

static uint32_t verb_data[] = {
#if 0
	0x00172001,
	0x001721e6,
	0x00172200,
	0x00172300,
#endif

	0x01471c10,
	0x01471d44,
	0x01471e01,
	0x01471f01,
//1
	0x01571c12,
	0x01571d14,
	0x01571e01,
	0x01571f01,
//2
	0x01671c11,
	0x01671d60,
	0x01671e01,
	0x01671f01,
//3
	0x01771c14,
	0x01771d20,
	0x01771e01,
	0x01771f01,
//4
	0x01871c30,
	0x01871d9c,
	0x01871ea1,
	0x01871f01,
//5
	0x01971c40,
	0x01971d9c,
	0x01971ea1,
	0x01971f02,
//6
	0x01a71c31,
	0x01a71d34,
	0x01a71e81,
	0x01a71f01,
//7
	0x01b71c1f,
	0x01b71d44,
	0x01b71e21,
	0x01b71f02,
//8
	0x01c71cf0,
	0x01c71d11,
	0x01c71e11,
	0x01c71f41,
//9
	0x01d71c3e,
	0x01d71d01,
	0x01d71e83,
	0x01d71f99,
//10
	0x01e71c20,
	0x01e71d41,
	0x01e71e45,
	0x01e71f01,
//11
	0x01f71c50,
	0x01f71d91,
	0x01f71ec5,
	0x01f71f01,
};

static unsigned find_verb(uint32_t viddid, uint32_t **verb)
{
	if(viddid != 0x10ec0880) return 0;
	*verb =  (uint32_t *)verb_data;
	return sizeof(verb_data)/sizeof(uint32_t);
}


static void codec_init(uint8_t *base, int addr)
{
	uint32_t dword;
	uint32_t *verb;
	unsigned verb_size;
	int i;

	/* 1 */
	do {
		dword = read32(base + 0x68);
	} while (dword & 1);

	dword = (addr<<28) | 0x000f0000;
	write32(base + 0x60, dword);

	do {
		dword = read32(base + 0x68);
	} while ((dword & 3)!=2);

	dword = read32(base + 0x64);

	/* 2 */
	printk(BIOS_DEBUG, "codec viddid: %08x\n", dword);
	verb_size = find_verb(dword, &verb);

	if(!verb_size) {
		printk(BIOS_DEBUG, "No verb!\n");
		return;
	}

	printk(BIOS_DEBUG, "verb_size: %d\n", verb_size);
	/* 3 */
	for(i=0; i<verb_size; i++) {
		do {
			dword = read32(base + 0x68);
		} while (dword & 1);

		write32(base + 0x60, verb[i]);

		do {
			dword = read32(base + 0x68);
		} while ((dword & 3) != 2);
	}
	printk(BIOS_DEBUG, "verb loaded!\n");
}

static void codecs_init(uint8_t *base, uint32_t codec_mask)
{
	int i;
	for(i=2; i>=0; i--) {
		if( codec_mask & (1<<i) )
			codec_init(base, i);
	}
}

static void aza_init(struct device *dev)
{
	uint8_t *base;
	struct resource *res;
	uint32_t codec_mask;

	res = find_resource(dev, 0x10);
	if(!res)
		return;

	base =(uint8_t *) res->base;
	printk(BIOS_DEBUG, "base = %08x\n", base);

	codec_mask = codec_detect(base);

	if(codec_mask) {
		printk(BIOS_DEBUG, "codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}
}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations aza_audio_ops  = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
//	.enable		= mcp55_enable,
	.init		= aza_init,
	.scan_bus	= 0,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver azaaudio_driver __pci_driver = {
	.ops	= &aza_audio_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.device	= PCI_DEVICE_ID_NVIDIA_MCP55_AZA,
};

