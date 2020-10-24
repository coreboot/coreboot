/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/azalia_device.h>
#include <device/mmio.h>
#include <delay.h>

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
	int count;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + HDA_GCTL_REG, 1, HDA_GCTL_CRST) < 0)
		goto no_codec;

	/* clear STATESTS bits (BAR + 0xe)[2:0] */
	reg32 = read32(base + HDA_STATESTS_REG);
	reg32 |= 7;
	write32(base + HDA_STATESTS_REG, reg32);

	/* Wait for readback of register to
	 * match what was just written to it
	 */
	count = 50;
	do {
		/* Wait 1ms based on BKDG wait time */
		mdelay(1);
		reg32 = read32(base + HDA_STATESTS_REG);
	} while ((reg32 != 0) && --count);
	/* Timeout occurred */
	if (!count)
		goto no_codec;

	/* Set Bit 0 to 0 to enter reset state (BAR + 0x8)[0] */
	if (set_bits(base + HDA_GCTL_REG, 1, 0) < 0)
		goto no_codec;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + HDA_GCTL_REG, 1, HDA_GCTL_CRST) < 0)
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
	printk(BIOS_DEBUG, "azalia_audio: No codec!\n");
	return 0;
}

static u32 find_verb(struct device *dev, u32 viddid, const u32 **verb)
{
	printk(BIOS_DEBUG, "azalia_audio: dev=%s\n", dev_path(dev));
	printk(BIOS_DEBUG, "azalia_audio: Reading viddid=%x\n", viddid);

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
	int timeout = 25;

	/* Send the verb to the codec */
	reg32 = read32(base + HDA_ICII_REG);
	reg32 |= HDA_ICII_BUSY | HDA_ICII_VALID;
	write32(base + HDA_ICII_REG, reg32);

	while (timeout--) {
		udelay(1);
	}
	timeout = 50;
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

	printk(BIOS_DEBUG, "azalia_audio: Initializing codec #%d\n", addr);

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
	printk(BIOS_DEBUG, "azalia_audio: codec viddid: %08x\n", reg32);
	verb_size = find_verb(dev, reg32, &verb);

	if (!verb_size) {
		printk(BIOS_DEBUG, "azalia_audio: No verb!\n");
		return;
	}
	printk(BIOS_DEBUG, "azalia_audio: verb_size: %d\n", verb_size);

	/* 3 */
	for (i = 0; i < verb_size; i++) {
		if (wait_for_ready(base) < 0)
			return;

		write32(base + HDA_IC_REG, verb[i]);

		if (wait_for_valid(base) < 0)
			return;
	}
	printk(BIOS_DEBUG, "azalia_audio: verb loaded.\n");
}

static void codecs_init(struct device *dev, u8 *base, u32 codec_mask)
{
	int i;

	for (i = 2; i >= 0; i--) {
		if (codec_mask & (1 << i))
			codec_init(dev, base, i);
	}
}

void azalia_audio_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	// NOTE this will break as soon as the azalia_audio get's a bar above 4G.
	// Is there anything we can do about it?
	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "azalia_audio: base = %p\n", base);
	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "azalia_audio: codec_mask = %02x\n", codec_mask);
		codecs_init(dev, base, codec_mask);
	}
}

struct device_operations default_azalia_audio_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= azalia_audio_init,
	.ops_pci		= &pci_dev_ops_pci,
};
