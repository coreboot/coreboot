/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <delay.h>
#include <device/azalia_device.h>
#include "chip.h"
#include "i82801gx.h"

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

	/* Wait for readback of register to match what was just written to it */
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
	u32 reg32;

	/* Set Bit 0 to 0 to enter reset state (BAR + 0x8)[0] */
	if (set_bits(base + HDA_GCTL_REG, HDA_GCTL_CRST, 0) < 0)
		goto no_codec;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + HDA_GCTL_REG, HDA_GCTL_CRST, HDA_GCTL_CRST) < 0)
		goto no_codec;

	/* Read in Codec location (BAR + 0xe)[2..0] */
	reg32 = read32(base + HDA_STATESTS_REG);
	reg32 &= 0x0f;
	if (!reg32)
		goto no_codec;

	return reg32;

no_codec:
	/* Codec Not found */
	/* Put HDA back in reset (BAR + 0x8) [0] */
	set_bits(base + HDA_GCTL_REG, 1, 0);
	printk(BIOS_DEBUG, "Azalia: No codec!\n");
	return 0;
}

static u32 find_verb(struct device *dev, u32 viddid, const u32 **verb)
{
	int idx = 0;

	while (idx < (cim_verb_data_size / sizeof(u32))) {
		u32 verb_size = 4 * cim_verb_data[idx + 2];	// in u32
		if (cim_verb_data[idx] != viddid) {
			idx += verb_size + 3;	// skip verb + header
			continue;
		}
		*verb = &cim_verb_data[idx + 3];
		return verb_size;
	}

	/* Not all codecs need to load another verb */
	return 0;
}

/*
 * Wait 50usec for the codec to indicate it is ready.
 * No response would imply that the codec is non-operative.
 */

static int wait_for_ready(u8 *base)
{
	/* Use a 50 usec timeout - the Linux kernel uses the same duration */
	int timeout = 50;

	while (timeout--) {
		u32 reg32 = read32(base + HDA_ICII_REG);
		if (!(reg32 & HDA_ICII_BUSY))
			return 0;
		udelay(1);
	}

	return -1;
}

/*
 * Wait 50usec for the codec to indicate that it accepted the previous command.
 * No response would imply that the code is non-operative.
 */

static int wait_for_valid(u8 *base)
{
	u32 reg32;
	/* Use a 50 usec timeout - the Linux kernel uses the same duration */
	int timeout = 50;

	/* Send the verb to the codec */
	reg32 = read32(base + HDA_ICII_REG);
	reg32 |= HDA_ICII_BUSY | HDA_ICII_VALID;
	write32(base + HDA_ICII_REG, reg32);

	while (timeout--) {
		reg32 = read32(base + HDA_ICII_REG);
		if ((reg32 & (HDA_ICII_VALID | HDA_ICII_BUSY)) == HDA_ICII_VALID)
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
	if (wait_for_ready(base) < 0) {
		printk(BIOS_DEBUG, "  codec not ready.\n");
		return;
	}

	reg32 = (addr << 28) | 0x000f0000;
	write32(base + HDA_IC_REG, reg32);

	if (wait_for_valid(base) < 0) {
		printk(BIOS_DEBUG, "  codec not valid.\n");
		return;
	}

	/* 2 */
	reg32 = read32(base + HDA_IR_REG);
	printk(BIOS_DEBUG, "Azalia: codec viddid: %08x\n", reg32);
	verb_size = find_verb(dev, reg32, &verb);

	if (!verb_size) {
		printk(BIOS_DEBUG, "Azalia: No verb!\n");
		return;
	}
	printk(BIOS_DEBUG, "Azalia: verb_size: %d\n", verb_size);

	/* 3 */
	for (i = 0; i < verb_size; i++) {
		if (wait_for_ready(base) < 0)
			return;

		write32(base + HDA_IC_REG, verb[i]);

		if (wait_for_valid(base) < 0)
			return;
	}
	printk(BIOS_DEBUG, "Azalia: verb loaded.\n");
}

static void codecs_init(struct device *dev, u8 *base, u32 codec_mask)
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

	// ESD
	pci_update_config32(dev, 0x134, ~(0xff << 16), 2 << 16);

	// Link1 description
	pci_update_config32(dev, 0x140, ~(0xff << 16), 2 << 16);

	// Port VC0 Resource Control Register
	pci_update_config32(dev, 0x114, ~(0xff << 0), 1);

	// VCi traffic class
	pci_or_config8(dev, 0x44, 7 << 0); // TC7

	// VCi Resource Control
	pci_or_config32(dev, 0x120, (1 << 31) | (1 << 24) | (0x80 << 0)); /* VCi ID and map */

	/* Set Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	pci_write_config8(dev, 0x3c, 0x0a); // unused?

	// TODO Actually check if we're AC97 or HDA instead of hardcoding this
	// here, in devicetree.cb and/or romstage.c.
	reg8 = pci_read_config8(dev, 0x40);
	reg8 |= (1 << 3); // Clear Clock Detect Bit
	pci_write_config8(dev, 0x40, reg8);
	reg8 &= ~(1 << 3); // Keep CLKDETCLR from clearing the bit over and over
	pci_write_config8(dev, 0x40, reg8);
	reg8 |= (1 << 2); // Enable clock detection
	pci_write_config8(dev, 0x40, reg8);
	mdelay(1);
	reg8 = pci_read_config8(dev, 0x40);
	printk(BIOS_DEBUG, "Azalia: codec type: %s\n", (reg8 & (1 << 1))?"Azalia":"AC97");

	// Select Azalia mode. This needs to be controlled via devicetree.cb
	pci_or_config8(dev, 0x40, 1); // Audio Control

	// Docking not supported
	pci_and_config8(dev, 0x4d, (u8)~(1 << 7)); // Docking Status

	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	// NOTE this will break as soon as the Azalia get's a bar above 4G.
	// Is there anything we can do about it?
	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "Azalia: base = %08x\n", (u32)base);
	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "Azalia: codec_mask = %02x\n", codec_mask);
		codecs_init(dev, base, codec_mask);
	}
}

static struct device_operations azalia_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= azalia_init,
	.enable			= i82801gx_enable,
	.ops_pci		= &pci_dev_ops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_azalia __pci_driver = {
	.ops	= &azalia_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d8,
};
