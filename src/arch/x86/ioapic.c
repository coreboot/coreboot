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
 */

#include <arch/io.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>

u32 io_apic_read(void *ioapic_base, u32 reg)
{
	write32(ioapic_base, reg);
	return read32(ioapic_base + 0x10);
}

void io_apic_write(void *ioapic_base, u32 reg, u32 value)
{
	write32(ioapic_base, reg);
	write32(ioapic_base + 0x10, value);
}

static int ioapic_interrupt_count(void *ioapic_base)
{
	/* Read the available number of interrupts. */
	int ioapic_interrupts = (io_apic_read(ioapic_base, 0x01) >> 16) & 0xff;
	if (ioapic_interrupts == 0xff)
		ioapic_interrupts = 23;
	ioapic_interrupts += 1; /* Bits 23-16 specify the maximum redirection
				   entry, which is the number of interrupts
				   minus 1. */
	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", ioapic_interrupts);

	return ioapic_interrupts;
}

void clear_ioapic(void *ioapic_base)
{
	u32 low, high;
	u32 i, ioapic_interrupts;

	printk(BIOS_DEBUG, "IOAPIC: Clearing IOAPIC at %p\n", ioapic_base);

	ioapic_interrupts = ioapic_interrupt_count(ioapic_base);

	low = INT_DISABLED;
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

void set_ioapic_id(void *ioapic_base, u8 ioapic_id)
{
	u32 bsp_lapicid = lapicid();
	int i;

	printk(BIOS_DEBUG, "IOAPIC: Initializing IOAPIC at 0x%p\n",
	       ioapic_base);
	printk(BIOS_DEBUG, "IOAPIC: Bootstrap Processor Local APIC = 0x%02x\n",
	       bsp_lapicid);

	if (ioapic_id) {
		printk(BIOS_DEBUG, "IOAPIC: ID = 0x%02x\n", ioapic_id);
		/* Set IOAPIC ID if it has been specified. */
		io_apic_write(ioapic_base, 0x00,
			(io_apic_read(ioapic_base, 0x00) & 0xf0ffffff) |
			(ioapic_id << 24));
	}

	printk(BIOS_SPEW, "IOAPIC: Dumping registers\n");
	for (i = 0; i < 3; i++)
		printk(BIOS_SPEW, "  reg 0x%04x: 0x%08x\n", i,
		       io_apic_read(ioapic_base, i));

}

static void load_vectors(void *ioapic_base)
{
	u32 bsp_lapicid = lapicid();
	u32 low, high;
	u32 i, ioapic_interrupts;

	ioapic_interrupts = ioapic_interrupt_count(ioapic_base);

	if (IS_ENABLED(CONFIG_IOAPIC_INTERRUPTS_ON_FSB)) {
		/*
		 * For the Pentium 4 and above APICs deliver their interrupts
		 * on the front side bus, enable that.
		 */
		printk(BIOS_DEBUG, "IOAPIC: Enabling interrupts on FSB\n");
		io_apic_write(ioapic_base, 0x03,
			      io_apic_read(ioapic_base, 0x03) | (1 << 0));
	} else if (IS_ENABLED(CONFIG_IOAPIC_INTERRUPTS_ON_APIC_SERIAL_BUS)) {
		printk(BIOS_DEBUG,
			"IOAPIC: Enabling interrupts on APIC serial bus\n");
		io_apic_write(ioapic_base, 0x03, 0);
	}

	/* Enable Virtual Wire Mode. */
	low = INT_ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
	high = bsp_lapicid << (56 - 32);

	io_apic_write(ioapic_base, 0x10, low);
	io_apic_write(ioapic_base, 0x11, high);

	if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
		printk(BIOS_WARNING, "IOAPIC not responding.\n");
		return;
	}

	printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n",
	       0, high, low);
	low = INT_DISABLED;
	high = NONE;
	for (i = 1; i < ioapic_interrupts; i++) {
		io_apic_write(ioapic_base, i * 2 + 0x10, low);
		io_apic_write(ioapic_base, i * 2 + 0x11, high);

		printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n",
		       i, high, low);
	}
}

void setup_ioapic(void *ioapic_base, u8 ioapic_id)
{
	set_ioapic_id(ioapic_base, ioapic_id);
	load_vectors(ioapic_base);
}
