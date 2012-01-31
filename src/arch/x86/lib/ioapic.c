/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>

static u32 io_apic_read(u32 ioapic_base, u32 reg)
{
	write32(ioapic_base, reg);
	return read32(ioapic_base + 0x10);
}

static void io_apic_write(u32 ioapic_base, u32 reg, u32 value)
{
	write32(ioapic_base, reg);
	write32(ioapic_base + 0x10, value);
}

/**
 * Clear all IOAPIC vectors.
 *
 * @param ioapic_base		IOAPIC base address
 */
void clear_ioapic(u32 ioapic_base)
{
	u32 low, high;
	u32 i, ioapic_interrupts;

	printk(BIOS_DEBUG, "IOAPIC: Clearing IOAPIC at 0x%08x\n", ioapic_base);

	/* Read the available number of interrupts. */
	ioapic_interrupts = (io_apic_read(ioapic_base, 0x01) >> 16) & 0xff;
	if (!ioapic_interrupts || ioapic_interrupts == 0xff)
		ioapic_interrupts = 24;
	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", ioapic_interrupts);

	low = DISABLED;
	high = NONE;

	for (i = 0; i < ioapic_interrupts; i++) {
		io_apic_write(ioapic_base, i * 2 + 0x10, low);
		io_apic_write(ioapic_base, i * 2 + 0x11, high);

		printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n",
		       i, high, low);
	}

	if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
		printk(BIOS_WARNING, "IOAPIC not responding.\n");
		return;
	}
}

/**
 * Assign IOAPIC with an ID and set delivery type.
 *
 * @param ioapic_base		IOAPIC base address
 * @param new_id		If non-zero (1-15), assign new apic ID.
 *				If zero, use previously assigned apic ID.
 * @param delivery		If 0x0, deliver interrupts on APIC serial bus
 * 				If 0x1, deliver interrupts on FSB
 */
static void init_ioapic(u32 ioapic_base, u8 new_id, u8 delivery)
{
	u8 loud = 0;
	u8 active_id;
	u32 reg32;

	reg32 = io_apic_read(ioapic_base, 0x00);
	active_id = (reg32 >> 24) & 0xf;

	/* Changing a previously assigned ID and attempting
	 * to use ID=0 here are logged at a critical level.
	 */
	if ( ((active_id && new_id) && (active_id != new_id)) ||
		!(active_id || new_id))
		loud = 1;

	if (!new_id) {
		printk(loud ? BIOS_CRIT : BIOS_DEBUG,
			"IOAPIC: 0x%08x using old ID = %d\n",
			ioapic_base, active_id);

	} else {
		reg32 &= 0xf0ffffff;
		reg32 |= (new_id & 0xf) << 24;
		io_apic_write(ioapic_base, 0x00, reg32);

		printk(loud ? BIOS_CRIT : BIOS_DEBUG,
			"IOAPIC: 0x%08x changing ID = %d->%d\n",
			ioapic_base, active_id, new_id);

		reg32 = io_apic_read(ioapic_base, 0x00);
		active_id = (reg32 >> 24) & 0xf;
		if (active_id != new_id)
			printk(BIOS_CRIT,
				"IOAPIC: 0x%08x changing ID failed (%d!=%d)\n",
				ioapic_base, active_id, new_id);
	}

	/* Assign interrupt delivery type. */
	reg32 = io_apic_read(ioapic_base, 0x03);
	switch (delivery) {
		case 0:
			printk(BIOS_DEBUG, "IOAPIC: Delivery is on APIC serial bus\n");
			reg32 = 0x00;
			break;
		case 1:
			printk(BIOS_DEBUG, "IOAPIC: Delivery is on FSB\n");
			reg32 = 0x01;
			break;
		default:
			printk(BIOS_CRIT, "IOAPIC: Delivery is reverted to FSB\n");
			reg32 = 0x01;
			break;
	}
	io_apic_write(ioapic_base, 0x03, reg32);
}

/**
 * Fill IOAPIC vectors all targeting the same processor.
 * Virtual Wire Mode on vector 0 is enabled, others remain disabled.
 *
 *
 * @param ioapic_base		IOAPIC base address
 * @param bsp_lapicid		APIC ID of a CPU to receive the interrupts
 */
static void load_vectors(u32 ioapic_base, u8 bsp_lapicid)
{
	u32 low, high;
	u32 i, ioapic_interrupts;

	/* Read the available number of interrupts. */
	ioapic_interrupts = (io_apic_read(ioapic_base, 0x01) >> 16) & 0xff;
	if (!ioapic_interrupts || ioapic_interrupts == 0xff)
		ioapic_interrupts = 24;
	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", ioapic_interrupts);

	/* Enable Virtual Wire Mode. */
	low = ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
	high = bsp_lapicid << (56 - 32);

	io_apic_write(ioapic_base, 0x10, low);
	io_apic_write(ioapic_base, 0x11, high);

	if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
		printk(BIOS_WARNING, "IOAPIC not responding.\n");
		return;
	}

	printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n",
	       0, high, low);
	low = DISABLED;
	high = NONE;
	for (i = 1; i < ioapic_interrupts; i++) {
		io_apic_write(ioapic_base, i * 2 + 0x10, low);
		io_apic_write(ioapic_base, i * 2 + 0x11, high);

		printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n",
		       i, high, low);
	}
}

/**
 * Assign IOAPIC with an ID
 *
 * Compile-time options from mainboard Kconfig can affect the
 * chosen IOAPIC operational mode.
 *
 * @param ioapic_base		IOAPIC base address
 * @param new_id		If non-zero (1-15), assign new apic ID.
 *				If zero, use previously assigned apic ID.
 */
void setup_ioapic(u32 ioapic_base, u8 new_id)
{
	u8 bsp_lapicid = lapicid();
	u8 ioapic_dt = CONFIG_IOAPIC_DELIVERY_TYPE;

	printk(BIOS_DEBUG, "IOAPIC: Initializing IOAPIC at 0x%08x\n",
	       ioapic_base);
	init_ioapic(ioapic_base, new_id, ioapic_dt);

	printk(BIOS_DEBUG, "IOAPIC: Bootstrap Processor Local APIC ID = %d\n",
	       bsp_lapicid);
	load_vectors(ioapic_base, bsp_lapicid);
}

void setup_ioapic_novectors(u32 ioapic_base, u8 new_id)
{
	u8 ioapic_dt = CONFIG_IOAPIC_DELIVERY_TYPE;

	printk(BIOS_DEBUG, "IOAPIC: Initializing IOAPIC at 0x%08x\n",
	       ioapic_base);
	init_ioapic(ioapic_base, new_id, ioapic_dt);

	printk(BIOS_CRIT, "IOAPIC: Interrupt Vectors were not initialized\n");
}

