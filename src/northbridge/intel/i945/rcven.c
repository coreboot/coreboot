/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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

#include "raminit.h"

/**
 * sample the strobes signal
 */
static u32 sample_strobes(int channel_offset, struct sys_info *sysinfo)
{
	u32 reg32, addr;
	int i;

	MCHBAR32(C0DRC1 + channel_offset) |= (1 << 6);

	MCHBAR32(C0DRC1 + channel_offset) &= ~(1 << 6);

	addr = 0;

	if (channel_offset != 0) {	/* must be dual channel */
		if (sysinfo->interleaved == 1) {
			addr |= (1 << 6);
		} else {
			addr = ((u32)MCHBAR8(C0DRB3)) << 25;
		}
	}

	for (i = 0; i < 28; i++) {
		read32(addr);
		read32(addr + 0x80);
	}

	reg32 = MCHBAR32(RCVENMT);
	if (channel_offset == 0) {
		reg32 = reg32 << 2;
	}

	/**
	 * [19] = 1: all bits are high
	 * [18] = 1: all bits are low
	 * [19:18] = 00: bits are mixed high, low
	 */
	return reg32;
}

/**
 * This function sets receive enable coarse and medium timing parameters
 */

static void set_receive_enable(int channel_offset, u8 medium, u8 coarse)
{
	u32 reg32;

	printk(BIOS_SPEW, "    set_receive_enable() medium=0x%x, coarse=0x%x\n", medium, coarse);

	reg32 = MCHBAR32(C0DRT1 + channel_offset);
	reg32 &= 0xf0ffffff;
	reg32 |= ((u32)coarse & 0x0f) << 24;
	MCHBAR32(C0DRT1 + channel_offset) = reg32;

	/* This should never happen: */
	if (coarse > 0x0f)
		printk(BIOS_DEBUG, "set_receive_enable: coarse overflow: 0x%02x.\n", coarse);

	/* medium control
	 *
	 * 00 - 1/4 clock
	 * 01 - 1/2 clock
	 * 10 - 3/4 clock
	 * 11 - 1   clock
	 */

	reg32 = MCHBAR32(RCVENMT);
	if (!channel_offset) {
		/* Channel 0 */
		reg32 &= ~(3 << 2);
		reg32 |= medium << 2;
	} else {
		/* Channel 1 */
		reg32 &= ~(3 << 0);
		reg32 |= medium;
	}
	MCHBAR32(RCVENMT) = reg32;

}

static int normalize(int channel_offset, u8 * mediumcoarse, u8 * fine)
{
	printk(BIOS_SPEW, "  normalize()\n");

	if (*fine < 0x80)
		return 0;

	*fine -= 0x80;
	*mediumcoarse += 1;

	if (*mediumcoarse >= 0x40) {
		printk(BIOS_DEBUG, "Normalize Error\n");
		return -1;
	}

	set_receive_enable(channel_offset, *mediumcoarse & 3,
			   *mediumcoarse >> 2);

	MCHBAR8(C0WL0REOST + channel_offset) = *fine;

	return 0;
}

static int find_preamble(int channel_offset, u8 * mediumcoarse,
			 struct sys_info *sysinfo)
{
	/* find start of the data phase */
	u32 reg32;

	printk(BIOS_SPEW, "  find_preamble()\n");

	do {
		if (*mediumcoarse < 4) {
			printk(BIOS_DEBUG, "No Preamble found.\n");
			return -1;
		}
		*mediumcoarse -= 4;

		set_receive_enable(channel_offset, *mediumcoarse & 3,
				   *mediumcoarse >> 2);

		reg32 = sample_strobes(channel_offset, sysinfo);

	} while (reg32 & (1 << 19));

	if (!(reg32 & (1 << 18))) {
		printk(BIOS_DEBUG, "No Preamble found (neither high nor low).\n");
		return -1;
	}

	return 0;
}

/**
 * add a quarter clock to the current receive enable settings
 */

static int add_quarter_clock(int channel_offset, u8 * mediumcoarse, u8 * fine)
{
	printk(BIOS_SPEW, "  add_quarter_clock() mediumcoarse=%02x fine=%02x\n",
			*mediumcoarse, *fine);
	if (*fine >= 0x80) {
		*fine -= 0x80;

		*mediumcoarse += 2;
		if (*mediumcoarse >= 0x40) {
			printk(BIOS_DEBUG, "clocks at max.\n");
			return -1;
		}

		set_receive_enable(channel_offset, *mediumcoarse & 3,
				   *mediumcoarse >> 2);
	} else {
		*fine += 0x80;
	}

	MCHBAR8(C0WL0REOST + channel_offset) = *fine;

	return 0;
}

static int find_strobes_low(int channel_offset, u8 * mediumcoarse, u8 * fine,
			    struct sys_info *sysinfo)
{
	u32 rcvenmt;

	printk(BIOS_SPEW, "  find_strobes_low()\n");

	for (;;) {
		MCHBAR8(C0WL0REOST + channel_offset) = *fine;

		set_receive_enable(channel_offset, *mediumcoarse & 3,
				   *mediumcoarse >> 2);

		rcvenmt = sample_strobes(channel_offset, sysinfo);

		if (((rcvenmt & (1 << 18)) != 0))
			return 0;

		*fine -= 0x80;
		if (*fine == 0)
			continue;

		*mediumcoarse -= 2;
		if (*mediumcoarse < 0xfe)
			continue;

		break;

	}

	printk(BIOS_DEBUG, "Could not find low strobe\n");
	return 0;
}

static int find_strobes_edge(int channel_offset, u8 * mediumcoarse, u8 * fine,
			     struct sys_info *sysinfo)
{

	int counter;
	u32 rcvenmt;

	printk(BIOS_SPEW, "  find_strobes_edge()\n");

	counter = 8;
	set_receive_enable(channel_offset, *mediumcoarse & 3,
			   *mediumcoarse >> 2);

	for (;;) {
		MCHBAR8(C0WL0REOST + channel_offset) = *fine;
		rcvenmt = sample_strobes(channel_offset, sysinfo);

		if ((rcvenmt & (1 << 19)) == 0) {
			counter = 8;
		} else {
			counter--;
			if (!counter)
				break;
		}

		*fine = *fine + 1;
		if (*fine < 0xf8) {
			if (*fine & (1 << 3)) {
				*fine &= ~(1 << 3);
				*fine += 0x10;
			}
			continue;
		}

		*fine = 0;
		*mediumcoarse += 2;
		if (*mediumcoarse <= 0x40) {
			set_receive_enable(channel_offset, *mediumcoarse & 3,
					   *mediumcoarse >> 2);
			continue;
		}

		printk(BIOS_DEBUG, "Could not find rising edge.\n");
		return -1;
	}

	*fine -= 7;
	if (*fine >= 0xf9) {
		*mediumcoarse -= 2;
		set_receive_enable(channel_offset, *mediumcoarse & 3,
				   *mediumcoarse >> 2);
	}

	*fine &= ~(1 << 3);
	MCHBAR8(C0WL0REOST + channel_offset) = *fine;

	return 0;
}

/**
 * Here we use a trick. The RCVEN channel 0 registers are all at an
 * offset of 0x80 to the channel 0 registers. We don't want to waste
 * a lot of if()s so let's just pass 0 or 0x80 for the channel offset.
 */

static int receive_enable_autoconfig(int channel_offset,
				     struct sys_info *sysinfo)
{
	u8 mediumcoarse;
	u8 fine;

	printk(BIOS_SPEW, "receive_enable_autoconfig() for channel %d\n",
		    channel_offset ? 1 : 0);

	/* Set initial values */
	mediumcoarse = (sysinfo->cas << 2) | 3;
	fine = 0;

	if (find_strobes_low(channel_offset, &mediumcoarse, &fine, sysinfo))
		return -1;

	if (find_strobes_edge(channel_offset, &mediumcoarse, &fine, sysinfo))
		return -1;

	if (add_quarter_clock(channel_offset, &mediumcoarse, &fine))
		return -1;

	if (find_preamble(channel_offset, &mediumcoarse, sysinfo))
		return -1;

	if (add_quarter_clock(channel_offset, &mediumcoarse, &fine))
		return -1;

	if (normalize(channel_offset, &mediumcoarse, &fine))
		return -1;

	/* This is a debug check to see if the rcven code is fully working.
	 * It can be removed when the output message is not printed anymore
	 */
	if (MCHBAR8(C0WL0REOST + channel_offset) == 0) {
		printk(BIOS_DEBUG, "Weird. No C%sWL0REOST\n", channel_offset?"1":"0");
	}

	return 0;
}

void receive_enable_adjust(struct sys_info *sysinfo)
{
	/* Is channel 0 populated? */
	if (sysinfo->dimm[0] != SYSINFO_DIMM_NOT_POPULATED
	    || sysinfo->dimm[1] != SYSINFO_DIMM_NOT_POPULATED)
		if (receive_enable_autoconfig(0, sysinfo))
			return;

	/* Is channel 1 populated? */
	if (sysinfo->dimm[2] != SYSINFO_DIMM_NOT_POPULATED
	    || sysinfo->dimm[3] != SYSINFO_DIMM_NOT_POPULATED)
		if (receive_enable_autoconfig(0x80, sysinfo))
			return;
}

