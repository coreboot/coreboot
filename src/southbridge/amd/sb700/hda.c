/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include "sb700.h"

#define HDA_ICII_REG 0x68
#define HDA_ICII_BUSY (1 << 0)
#define HDA_ICII_VALID (1 << 1)

static int set_bits(void *port, u32 mask, u32 val)
{
	u32 dword;
	int count;

	/* Write (val & ~mask) to port */
	val &= mask;
	dword = read32(port);
	dword &= ~mask;
	dword |= val;
	write32(port, dword);

	/* Wait for readback of register to
	 * match what was just written to it
	 */
	count = 50;
	do {
		/* Wait 1ms based on BKDG wait time */
		mdelay(1);
		dword = read32(port);
		dword &= mask;
	} while ((dword != val) && --count);

	/* Timeout occurred */
	if (!count)
		return -1;
	return 0;
}

static u32 codec_detect(void *base)
{
	u32 dword;

	/* Set Bit0 to 0 to enter reset state (BAR + 0x8)[0] */
	if (set_bits(base + 0x08, 1, 0) == -1)
		goto no_codec;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + 0x08, 1, 1) == -1)
		goto no_codec;

	/* Delay for 1 ms since the BKDG does */
	mdelay(1);

	/* Read in Codec location (BAR + 0xe)[3..0]*/
	dword = read32(base + 0xe);
	dword &= 0x0F;
	if (!dword)
		goto no_codec;

	return dword;

no_codec:
	/* Codec Not found */
	/* Put HDA back in reset (BAR + 0x8) [0] */
	set_bits(base + 0x08, 1, 0);
	printk(BIOS_DEBUG, "No codec!\n");
	return 0;
}

/**
 *  Wait 50usec for the codec to indicate it is ready
 *  no response would imply that the codec is non-operative
 */
static int wait_for_ready(void *base)
{
	/* Use a 50 usec timeout - the Linux kernel uses the
	 * same duration */

	int timeout = 50;

	while (timeout--) {
		u32 dword = read32(base +  HDA_ICII_REG);
		if (!(dword & HDA_ICII_BUSY))
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
static int wait_for_valid(void *base)
{
	/* Use a 50 usec timeout - the Linux kernel uses the
	 * same duration */

	int timeout = 50;
	while (timeout--) {
		u32 dword = read32(base + HDA_ICII_REG);
		if ((dword & (HDA_ICII_VALID | HDA_ICII_BUSY)) ==
			HDA_ICII_VALID)
			return 0;
		udelay(1);
	}

	return -1;
}

static void codec_init(void *base, int addr)
{
	u32 dword;

	/* 1 */
	if (wait_for_ready(base) == -1)
		return;

	dword = (addr << 28) | 0x000f0000;
	write32(base + 0x60, dword);

	if (wait_for_valid(base) == -1)
		return;

	dword = read32(base + 0x64);

	/* 2 */
	printk(BIOS_DEBUG, "%x(th) codec viddid: %08x\n", addr, dword);
}

static void codecs_init(void *base, u32 codec_mask)
{
	int i;
	for (i = 2; i >= 0; i--) {
		if (codec_mask & (1 << i))
			codec_init(base, i);
	}
}

static void hda_init(struct device *dev)
{
	u8 byte;
	u32 dword;
	void *base;
	struct resource *res;
	u32 codec_mask;
	struct device *sm_dev;

	/* Enable azalia - PM_io 0x59[3], no ac97 in sb700. */
	byte = pm_ioread(0x59);
	byte |= 1 << 3;
	pm_iowrite(0x59, byte);

	/* Find the SMBus */
	sm_dev = dev_find_device(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB700_SM, 0);

	/* Set routing pin - SMBus ExtFunc (0xf8/0xfc) */
	pci_write_config32(sm_dev, 0xf8, 0x00);
	pci_write_config8(sm_dev, 0xfc, 0xAA);
	/* Set INTA - SMBus 0x63 [2..0] */
	byte = pci_read_config8(sm_dev, 0x63);
	byte &= ~0x7;
	byte |= 0x0; /* INTA:0x0 - INTH:0x7 */
	pci_write_config8(sm_dev, 0x63, byte);

	/* Program the 2C to 0x437b1002 */
	dword = 0x437b1002;
	pci_write_config32(dev, 0x2c, dword);

	/* Read in BAR */
	/* Is this right? HDA allows for a 64-bit BAR
	 * but this is only setup for a 32-bit one
	 */
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
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations hda_audio_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = hda_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver hdaaudio_driver __pci_driver = {
	.ops = &hda_audio_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_HDA,
};
