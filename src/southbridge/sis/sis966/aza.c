/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "sis966.h"

u8	SiS_SiS7502_init[7][3]={
{0x04, 0xFF, 0x07},
{0x2C, 0xFF, 0x39},
{0x2D, 0xFF, 0x10},
{0x2E, 0xFF, 0x91},
{0x2F, 0xFF, 0x01},
{0x04, 0xFF, 0x06},
{0x00, 0x00, 0x00}					//End of table
};

static int set_bits(void *port, u32 mask, u32 val)
{
	u32 dword;
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

	if (!count) return -1;

	udelay(500);
	return 0;

}

static u32 send_verb(u8 *base, u32 verb)
{
	u32 dword;

	dword = read32(base + 0x68);
	dword = dword|(unsigned long)0x0002;
	write32(base + 0x68, dword);
	do {
		dword = read32(base + 0x68);
	} while ((dword & 1) != 0);
	write32(base + 0x60, verb);
	udelay(500);
	dword = read32(base + 0x68);
	dword = (dword |0x1);
	write32(base + 0x68, dword);
	do {
		udelay(100);
		dword = read32(base + 0x68);
	} while ((dword & 3) != 2);

	dword = read32(base + 0x64);
	return dword;
}


static int codec_detect(u8 *base)
{
	u32 dword;
	int idx = 0;

	/* 1 */ // controller reset
	printk(BIOS_DEBUG, "controller reset\n");

	set_bits(base + 0x08, 1, 1);

	do {
		dword = read32(base + 0x08)&0x1;
		if (idx++>1000) { printk(BIOS_DEBUG, "controller reset fail !!!\n"); break;}
	} while (dword !=1);

	dword=send_verb(base,0x000F0000); // get codec VendorId and DeviceId

	if (dword == 0) {
		printk(BIOS_DEBUG, "No codec!\n");
		return 0;
	}

	 printk(BIOS_DEBUG, "Codec ID = %x\n", dword);

	dword = 0x1;
	return dword;

}


static u32 verb_data[] = {

//14
	0x01471c10,
	0x01471d40,
	0x01471e01,
	0x01471f01,
//15
	0x01571c12,
	0x01571d10,
	0x01571e01,
	0x01571f01,
//16
	0x01671c11,
	0x01671d60,
	0x01671e01,
	0x01671f01,
//17
	0x01771c14,
	0x01771d20,
	0x01771e01,
	0x01771f01,
//18
	0x01871c40,
	0x01871d98,
	0x01871ea1,
	0x01871f01,
//19
	0x01971c50,
	0x01971d98,
	0x01971ea1,
	0x01971f02,
//1a
	0x01a71c4f,
	0x01a71d30,
	0x01a71e81,
	0x01a71f01,
//1b
	0x01b71c20,
	0x01b71d40,
	0x01b71e01,
	0x01b71f02,
//1c
	0x01c71cf0,
	0x01c71d01,
	0x01c71e33,
	0x01c71f59,
//1d
	0x01d71c01,
	0x01d71de6,
	0x01d71e05,
	0x01d71f40,
//1e
	0x01e71c30,
	0x01e71d11,
	0x01e71e44,
	0x01e71f01,
//1f
	0x01f71c60,
	0x01f71d61,
	0x01f71ec4,
	0x01f71f01,
};

static unsigned find_verb(u32 viddid, u32 **verb)
{
	if ((viddid == 0x10ec0883) || (viddid == 0x10ec0882) || (viddid == 0x10ec0880)) return 0;
	*verb = (u32 *)verb_data;
	return sizeof(verb_data)/sizeof(u32);
}


static void codec_init(u8 *base, int addr)
{
	u32 dword;
	u32 *verb;
	unsigned verb_size;
	int i;

	/* 1 */
	do {
		dword = read32(base + 0x68);
	} while (dword & 1);

	dword = (addr << 28) | 0x000f0000;
	write32(base + 0x60, dword);

	do {
		dword = read32(base + 0x68);
	} while ((dword & 3) != 2);

	dword = read32(base + 0x64);

	/* 2 */
	printk(BIOS_DEBUG, "codec viddid: %08x\n", dword);
	verb_size = find_verb(dword, &verb);

	if (!verb_size) {
		printk(BIOS_DEBUG, "No verb!\n");
		return;
	}

	printk(BIOS_DEBUG, "verb_size: %d\n", verb_size);
	/* 3 */
	for (i=0; i<verb_size; i++) {
		send_verb(base,verb[i]);
	}
	printk(BIOS_DEBUG, "verb loaded!\n");
}

static void codecs_init(u8 *base, u32 codec_mask)
{
	codec_init(base, 0);
	return;
}

static void aza_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	printk(BIOS_DEBUG, "AZALIA_INIT:---------->\n");

//-------------- enable AZA (SiS7502) -------------------------
{
	u8 temp8;
	int i=0;
	while (SiS_SiS7502_init[i][0] != 0)
	{
		temp8 = pci_read_config8(dev, SiS_SiS7502_init[i][0]);
		temp8 &= SiS_SiS7502_init[i][1];
		temp8 |= SiS_SiS7502_init[i][2];
		pci_write_config8(dev, SiS_SiS7502_init[i][0], temp8);
		i++;
	};
}
//-----------------------------------------------------------


	// put audio to D0 state
	pci_write_config8(dev, 0x54,0x00);

#if DEBUG_AZA
{
	int i;

	printk(BIOS_DEBUG, "****** Azalia PCI config ******");
	printk(BIOS_DEBUG, "\n    03020100  07060504  0B0A0908  0F0E0D0C");

	for (i=0; i<0xff; i+=4){
		if ((i%16)==0){
			printk(BIOS_DEBUG, "\n%02x: ", i);
		}
		printk(BIOS_DEBUG, "%08x  ", pci_read_config32(dev,i));
	}
	printk(BIOS_DEBUG, "\n");
}
#endif

	res = find_resource(dev, 0x10);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "base = 0x%p\n", base);

	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}

	printk(BIOS_DEBUG, "AZALIA_INIT:<----------\n");
}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations aza_audio_ops = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
//	.enable		= sis966_enable,
	.init		= aza_init,
	.scan_bus	= 0,
	.ops_pci	= &lops_pci,
};

static const struct pci_driver azaaudio_driver __pci_driver = {
	.ops	= &aza_audio_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_HD_AUDIO,
};
