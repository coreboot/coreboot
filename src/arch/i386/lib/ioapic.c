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

void setup_ioapic(u32 ioapic_base, u8 ioapic_id)
{
	u32 bsp_lapicid = lapicid();
	u32 low, high;
	u32 i, ioapic_interrupts;

	printk(BIOS_DEBUG, "IOAPIC: Initializing IOAPIC at 0x%08x\n",
	       ioapic_base);
	printk(BIOS_DEBUG, "IOAPIC: Bootstrap Processor Local APIC = 0x%02x\n",
	       bsp_lapicid);

	if (ioapic_id) {
		printk(BIOS_DEBUG, "IOAPIC: ID = 0x%02x\n", ioapic_id);
		/* Set IOAPIC ID if it has been specified. */
		io_apic_write(ioapic_base, 0x00,
			(io_apic_read(ioapic_base, 0x00) & 0xfff0ffff) |
			(ioapic_id << 24));
	}

	/* Read the available number of interrupts. */
	ioapic_interrupts = (io_apic_read(ioapic_base, 0x01) >> 16) & 0xff;
	if (!ioapic_interrupts || ioapic_interrupts == 0xff)
		ioapic_interrupts = 24;
	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", ioapic_interrupts);

// XXX this decision should probably be made elsewhere, and
// it's the C3, not the EPIA this depends on.
#if defined(CONFIG_EPIA_VT8237R_INIT) && CONFIG_EPIA_VT8237R_INIT
#define IOAPIC_INTERRUPTS_ON_APIC_SERIAL_BUS
#else
#define IOAPIC_INTERRUPTS_ON_FSB
#endif

#ifdef IOAPIC_INTERRUPTS_ON_FSB
	/*
	 * For the Pentium 4 and above APICs deliver their interrupts
	 * on the front side bus, enable that.
	 */
	printk(BIOS_DEBUG, "IOAPIC: Enabling interrupts on FSB\n");
	io_apic_write(ioapic_base, 0x03,
		      io_apic_read(ioapic_base, 0x03) | (1 << 0));
#endif
#ifdef IOAPIC_INTERRUPTS_ON_APIC_SERIAL_BUS
	printk(BIOS_DEBUG, "IOAPIC: Enabling interrupts on APIC serial bus\n");
	io_apic_write(ioapic_base, 0x03, 0);
#endif

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
