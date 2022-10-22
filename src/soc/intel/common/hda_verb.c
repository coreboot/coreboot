/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/azalia_device.h>
#include <device/mmio.h>
#include <stdint.h>

#include "hda_verb.h"

int hda_codec_detect(u8 *base)
{
	u8 reg8;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (azalia_exit_reset(base) < 0)
		goto no_codec;

	/* Write back the value once reset bit is set. */
	write16(base + HDA_GCAP_REG, read16(base + HDA_GCAP_REG));

	/*
	 * Clear the "State Change Status Register" STATESTS bits
	 * for each of the "SDIN Stat Change Status Flag"
	 */
	write8(base + HDA_STATESTS_REG, 0xf);

	/* Turn off the link and poll RESET# bit until it reads back as 0 */
	if (azalia_enter_reset(base) < 0)
		goto no_codec;

	/* Turn on the link and poll RESET# bit until it reads back as 1 */
	if (azalia_exit_reset(base) < 0)
		goto no_codec;

	/* Read in Codec location (BAR + 0xe)[2..0] */
	reg8 = read8(base + HDA_STATESTS_REG);
	reg8 &= 0x0f;
	if (!reg8)
		goto no_codec;

	return reg8;

no_codec:
	/* Codec not found, put HDA back in reset */
	azalia_enter_reset(base);
	printk(BIOS_DEBUG, "HDA: No codec!\n");
	return 0;
}

/*
 * Wait 50usec for the codec to indicate it is ready.
 * No response would imply that the codec is non-operative.
 */
static int hda_wait_for_ready(u8 *base)
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
static int hda_wait_for_valid(u8 *base)
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

int hda_codec_write(u8 *base, u32 size, const u32 *data)
{
	int i;

	for (i = 0; i < size; i++) {
		if (hda_wait_for_ready(base) < 0)
			return -1;

		write32(base + HDA_IC_REG, data[i]);

		if (hda_wait_for_valid(base) < 0)
			return -1;
	}

	return 0;
}

int hda_codec_init(u8 *base, int addr, int verb_size, const u32 *verb_data)
{
	const u32 *verb;
	u32 reg32, size;
	int rc;

	printk(BIOS_DEBUG, "HDA: Initializing codec #%d\n", addr);

	if (!verb_size || !verb_data) {
		printk(BIOS_DEBUG, "HDA: No verb list!\n");
		return -1;
	}

	/* 1 */
	if (hda_wait_for_ready(base) < 0) {
		printk(BIOS_DEBUG, "  codec not ready.\n");
		return -1;
	}

	reg32 = (addr << 28) | 0x000f0000;
	write32(base + HDA_IC_REG, reg32);

	if (hda_wait_for_valid(base) < 0) {
		printk(BIOS_DEBUG, "  codec not valid.\n");
		return -1;
	}

	/* 2 */
	reg32 = read32(base + HDA_IR_REG);
	printk(BIOS_DEBUG, "HDA: codec viddid: %08x\n", reg32);

	size = azalia_find_verb(verb_data, verb_size, reg32, &verb);
	if (!size) {
		printk(BIOS_DEBUG, "HDA: No verb table entry found\n");
		return -1;
	}

	/* 3 */
	rc = hda_codec_write(base, size, verb);

	if (rc < 0)
		printk(BIOS_DEBUG, "HDA: verb not loaded\n");
	else
		printk(BIOS_DEBUG, "HDA: verb loaded.\n");

	return rc;
}
