/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include "pch.h"
#include "hda_verb.h"

/**
 * Set bits in a register and wait for status
 */
static int set_bits(u32 port, u32 mask, u32 val)
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

/**
 * Probe for supported codecs
 */
int hda_codec_detect(u32 base)
{
	u8 reg8;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (set_bits(base + HDA_GCTL_REG, HDA_GCTL_CRST, HDA_GCTL_CRST) < 0)
		goto no_codec;

	/* Write back the value once reset bit is set. */
	write16(base + HDA_GCAP_REG, read16(base + HDA_GCAP_REG));

	/* Read in Codec location (BAR + 0xe)[2..0]*/
	reg8 = read8(base + HDA_STATESTS_REG);
	reg8 &= 0x0f;
	if (!reg8)
		goto no_codec;

	return reg8;

no_codec:
	/* Codec Not found */
	/* Put HDA back in reset (BAR + 0x8) [0] */
	set_bits(base + HDA_GCTL_REG, HDA_GCTL_CRST, 0);
	printk(BIOS_DEBUG, "HDA: No codec!\n");
	return 0;
}

/**
 * Wait 50usec for the codec to indicate it is ready
 * no response would imply that the codec is non-operative
 */
static int hda_wait_for_ready(u32 base)
{
	/* Use a 50 usec timeout - the Linux kernel uses the
	 * same duration */

	int timeout = 50;

	while(timeout--) {
		u32 reg32 = read32(base +  HDA_ICII_REG);
		if (!(reg32 & HDA_ICII_BUSY))
			return 0;
		udelay(1);
	}

	return -1;
}

/**
 * Wait 50usec for the codec to indicate that it accepted
 * the previous command.  No response would imply that the code
 * is non-operative
 */
static int hda_wait_for_valid(u32 base)
{
	u32 reg32;

	/* Send the verb to the codec */
	reg32 = read32(base + HDA_ICII_REG);
	reg32 |= HDA_ICII_BUSY | HDA_ICII_VALID;
	write32(base + HDA_ICII_REG, reg32);

	/* Use a 50 usec timeout - the Linux kernel uses the
	 * same duration */

	int timeout = 50;
	while(timeout--) {
		reg32 = read32(base + HDA_ICII_REG);
		if ((reg32 & (HDA_ICII_VALID | HDA_ICII_BUSY)) ==
			HDA_ICII_VALID)
			return 0;
		udelay(1);
	}

	return -1;
}

/**
 * Find a specific entry within a verb table
 *
 * @verb_table_bytes:	verb table size in bytes
 * @verb_table_data:	verb table data
 * @viddid:		vendor/device to search for
 * @verb_out:		pointer to entry within table
 *
 * Returns size of the entry within the verb table,
 * Returns 0 if the entry is not found
 *
 * The HDA verb table is composed of dwords. A set of 4 dwords is
 * grouped together to form a "jack" descriptor.
 *   Bits 31:28 - Codec Address
 *   Bits 27:20 - NID
 *   Bits 19:8  - Verb ID
 *   Bits 7:0   - Payload
 *
 * coreboot groups different codec verb tables into a single table
 * and prefixes each with a specific header consisting of 3
 * dword entries:
 *   1 - Codec Vendor/Device ID
 *   2 - Subsystem ID
 *   3 - Number of jacks (groups of 4 dwords) for this codec
 */
static u32 hda_find_verb(u32 verb_table_bytes,
			 const u32 *verb_table_data,
			 u32 viddid, const u32 ** verb)
{
	int idx=0;

	while (idx < (verb_table_bytes / sizeof(u32))) {
		u32 verb_size = 4 * verb_table_data[idx+2]; // in u32
		if (verb_table_data[idx] != viddid) {
			idx += verb_size + 3; // skip verb + header
			continue;
		}
		*verb = &verb_table_data[idx+3];
		return verb_size;
	}

	/* Not all codecs need to load another verb */
	return 0;
}

/**
 * Write a supplied verb table
 */
int hda_codec_write(u32 base, u32 size, const u32 *data)
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

/**
 * Initialize codec, then find the verb table and write it
 */
int hda_codec_init(u32 base, int addr, int verb_size, const u32 *verb_data)
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

	size = hda_find_verb(verb_size, verb_data, reg32, &verb);
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
