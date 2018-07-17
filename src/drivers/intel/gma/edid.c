/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
 * Copyright (C) 2016 Sebastian Grzywna <swiftgeek@gmail.com>
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
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>

#include "i915_reg.h"
#include "edid.h"
#define GMBUS0_ADDR (mmio + 4 * 0)
#define GMBUS1_ADDR (mmio + 4 * 1)
#define GMBUS2_ADDR (mmio + 4 * 2)
#define GMBUS3_ADDR (mmio + 4 * 3)
#define GMBUS5_ADDR (mmio + 4 * 8)
#define AT24_ADDR 0x50 /* EDID EEPROM */

static void wait_rdy(u8 *mmio)
{
	unsigned try = 100;

	while (try--) {
		if (read32(GMBUS2_ADDR) & GMBUS_HW_RDY)
			return;
		udelay(10);
	}
}

static void intel_gmbus_stop_bus(u8 *mmio, u8 bus)
{
	wait_rdy(mmio);
	write32(GMBUS0_ADDR, bus);
	wait_rdy(mmio);
	write32(GMBUS5_ADDR, 0);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY | GMBUS_CYCLE_WAIT | GMBUS_CYCLE_INDEX
		| GMBUS_CYCLE_STOP | ( 0x4 << GMBUS_BYTE_COUNT_SHIFT )
		| GMBUS_SLAVE_READ | (AT24_ADDR << 1) );
	wait_rdy(mmio);
	write32(GMBUS5_ADDR, 0);
	write32(GMBUS1_ADDR, GMBUS_SW_CLR_INT);
	write32(GMBUS1_ADDR, 0);
	wait_rdy(mmio);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY | GMBUS_CYCLE_STOP | GMBUS_SLAVE_WRITE
		| (AT24_ADDR << 1) );
	wait_rdy(mmio);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY | GMBUS_CYCLE_STOP);
	write32(GMBUS2_ADDR, GMBUS_INUSE);
}

void intel_gmbus_stop(u8 *mmio)
{
	intel_gmbus_stop_bus(mmio, 6);
	intel_gmbus_stop_bus(mmio, 2);
}

void intel_gmbus_read_edid(u8 *mmio, u8 bus, u8 slave, u8 *edid, u32 edid_size)
{
	int i;

	slave &= 0x7f;
	edid_size &= 0x1fc;

	wait_rdy(mmio);
	/* 100 KHz, hold 0ns,  */
	write32(GMBUS0_ADDR, bus);
	wait_rdy(mmio);
	/* Ensure index bits are disabled.  */
	write32(GMBUS5_ADDR, 0);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY | GMBUS_CYCLE_WAIT | GMBUS_CYCLE_INDEX
		| (slave << 1) );
	wait_rdy(mmio);
	/* Ensure index bits are disabled.  */
	write32(GMBUS5_ADDR, 0);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY | GMBUS_SLAVE_READ | GMBUS_CYCLE_WAIT
		| GMBUS_CYCLE_STOP
		| (edid_size << GMBUS_BYTE_COUNT_SHIFT) | (slave << 1) );
	for (i = 0; i < edid_size / 4; i++) {
		u32 reg32;
		wait_rdy(mmio);
		reg32 = read32(GMBUS3_ADDR);
		edid[4 * i] = reg32 & 0xff;
		edid[4 * i + 1] = (reg32 >> 8) & 0xff;
		edid[4 * i + 2] = (reg32 >> 16) & 0xff;
		edid[4 * i + 3] = (reg32 >> 24) & 0xff;
	}
	wait_rdy(mmio);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY
		| GMBUS_SLAVE_WRITE | GMBUS_CYCLE_WAIT | GMBUS_CYCLE_STOP
		| (128 << GMBUS_BYTE_COUNT_SHIFT) | (slave << 1) );
	wait_rdy(mmio);
	write32(GMBUS1_ADDR, GMBUS_SW_RDY | GMBUS_CYCLE_STOP );
	write32(GMBUS2_ADDR, GMBUS_INUSE);

	printk (BIOS_SPEW, "EDID:\n");
	for (i = 0; i < 128; i++) {
		printk (BIOS_SPEW, "%02x ", edid[i]);
		if ((i & 0xf) == 0xf)
			printk (BIOS_SPEW, "\n");
	}
}
