/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2, or (at your option)
 * any later verion of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>

#include "i915_reg.h"
#include "edid.h"

static void wait_rdy(u8 *mmio)
{
	unsigned try = 100;

	while (try--) {
		if (read32(mmio + 8) & (1 << 11))
			return;
		udelay(10);
	}
}

void intel_gmbus_read_edid(u8 *mmio, u8 bus, u8 slave, u8 *edid, u32 edid_size)
{
	int i;

	slave &= 0x7f;
	edid_size &= 0x1fc;

	wait_rdy(mmio);
	/* 100 KHz, hold 0ns,  */
	write32(mmio + 4 * 0, bus);
	wait_rdy(mmio);
	/* Ensure index bits are disabled.  */
	write32(mmio + 4 * 5, 0);
	write32(mmio + 4 * 1, 0x46000000 | (slave << 1));
	wait_rdy(mmio);
	/* Ensure index bits are disabled.  */
	write32(mmio + 4 * 5, 0);
	write32(mmio + 4 * 1, 0x4a000001 | (slave << 1)
		| (edid_size << 16));
	for (i = 0; i < edid_size / 4; i++) {
		u32 reg32;
		wait_rdy(mmio);
		reg32 = read32(mmio + 4 * 3);
		edid[4 * i] = reg32 & 0xff;
		edid[4 * i + 1] = (reg32 >> 8) & 0xff;
		edid[4 * i + 2] = (reg32 >> 16) & 0xff;
		edid[4 * i + 3] = (reg32 >> 24) & 0xff;
	}
	wait_rdy(mmio);
	write32(mmio + 4 * 1, 0x4a800000 | (slave << 1));
	wait_rdy(mmio);
	write32(mmio + 4 * 0, 0x48000000);
	write32(mmio + 4 * 2, 0x00008000);

	printk (BIOS_SPEW, "EDID:\n");
	for (i = 0; i < 128; i++) {
		printk (BIOS_SPEW, "%02x ", edid[i]);
		if ((i & 0xf) == 0xf)
			printk (BIOS_SPEW, "\n");
	}
}
