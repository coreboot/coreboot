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

#include "i915.h"
#include "i915_reg.h"
#include "edid.h"

static void
wait_rdy(void)
{
	unsigned try = 100;

	while (try--) {
		if (io_i915_read32(PCH_GMBUS2) & (1 << 11))
			return;
		udelay(10);
	}
}

void
intel_gmbus_read_edid(u8 bus, u8 slave, u8 *edid)
{
	int i;

	wait_rdy();
	/* 100 KHz, hold 0ns,  */
	io_i915_write32(bus, PCH_GMBUS0);
	wait_rdy();
	/* Ensure index bits are disabled.  */
	io_i915_write32(0, PCH_GMBUS5);
	io_i915_write32(0x46000000 | (slave << 1), PCH_GMBUS1);
	wait_rdy();
	/* Ensure index bits are disabled.  */
	io_i915_write32(0, PCH_GMBUS5);
	io_i915_write32(0x4a800001 | (slave << 1), PCH_GMBUS1);
	for (i = 0; i < 128 / 4; i++) {
		u32 reg32;
		wait_rdy();
		reg32 = io_i915_read32(PCH_GMBUS3);
		edid[4 * i] = reg32 & 0xff;
		edid[4 * i + 1] = (reg32 >> 8) & 0xff;
		edid[4 * i + 2] = (reg32 >> 16) & 0xff;
		edid[4 * i + 3] = (reg32 >> 24) & 0xff;
	}
	wait_rdy();
	io_i915_write32(0x4a800000 | (slave << 1), PCH_GMBUS1);
	wait_rdy();
	io_i915_write32(0x48000000, PCH_GMBUS0);
	io_i915_write32(0x00008000, PCH_GMBUS2);

	printk (BIOS_INFO, "EDID:\n");
	for (i = 0; i < 128; i++) {
		printk (BIOS_INFO, "%02x ", edid[i]);
		if ((i & 0xf) == 0xf)
			printk (BIOS_INFO, "\n");
	}
}
