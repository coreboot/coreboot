/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/mmio.h>
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

static void write_vector(void *ioapic_base, u8 vector, u32 high, u32 low)
{
	io_apic_write(ioapic_base, vector * 2 + 0x10, low);
	io_apic_write(ioapic_base, vector * 2 + 0x11, high);

	printk(BIOS_SPEW, "IOAPIC: vector 0x%02x value 0x%08x 0x%08x\n",
	       vector, high, low);
}

static int ioapic_interrupt_count(void *ioapic_base)
{
	/* Read the available number of interrupts. */
	int ioapic_interrupts = (io_apic_read(ioapic_base, 0x01) >> 16) & 0xff;
	if (!ioapic_interrupts || ioapic_interrupts == 0xff)
		ioapic_interrupts = 23;
	ioapic_interrupts += 1; /* Bits 23-16 specify the maximum redirection
				   entry, which is the number of interrupts
				   minus 1. */
	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", ioapic_interrupts);

	return ioapic_interrupts;
}

static void clear_vectors(void *ioapic_base, u8 first, u8 last)
{
	u32 low, high;
	u8 i;

	printk(BIOS_DEBUG, "IOAPIC: Clearing IOAPIC at %p\n", ioapic_base);

	low = INT_DISABLED;
	high = NONE;

	for (i = first; i <= last; i++)
		write_vector(ioapic_base, i, high, low);

	if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
		printk(BIOS_WARNING, "IOAPIC not responding.\n");
		return;
	}
}

void clear_ioapic(void *ioapic_base)
{
	clear_vectors(ioapic_base, 0, ioapic_interrupt_count(ioapic_base) - 1);
}

static void route_i8259_irq0(void *ioapic_base)
{
	u32 bsp_lapicid = lapicid();
	u32 low, high;

	ASSERT(bsp_lapicid < 255);

	printk(BIOS_DEBUG, "IOAPIC: Bootstrap Processor Local APIC = 0x%02x\n",
	       bsp_lapicid);

	/* Enable Virtual Wire Mode. Should this be LOGICAL_DEST instead? */
	low = INT_ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
	high = bsp_lapicid << (56 - 32);

	write_vector(ioapic_base, 0, high, low);

	if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
		printk(BIOS_WARNING, "IOAPIC not responding.\n");
		return;
	}
}

void set_ioapic_id(void *ioapic_base, u8 ioapic_id)
{
	int i;

	printk(BIOS_DEBUG, "IOAPIC: Initializing IOAPIC at %p\n",
	       ioapic_base);

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

u8 get_ioapic_id(void *ioapic_base)
{
	return (io_apic_read(ioapic_base, 0x00) >> 24) & 0x0f;
}

u8 get_ioapic_version(void *ioapic_base)
{
	return io_apic_read(ioapic_base, 0x01) & 0xff;
}

void ioapic_set_boot_config(void *ioapic_base, bool irq_on_fsb)
{
	if (irq_on_fsb) {
		/*
		 * For the Pentium 4 and above APICs deliver their interrupts
		 * on the front side bus, enable that.
		 */
		printk(BIOS_DEBUG, "IOAPIC: Enabling interrupts on FSB\n");
		io_apic_write(ioapic_base, 0x03,
			      io_apic_read(ioapic_base, 0x03) | (1 << 0));
	} else {
		printk(BIOS_DEBUG,
			"IOAPIC: Enabling interrupts on APIC serial bus\n");
		io_apic_write(ioapic_base, 0x03, 0);
	}
}

void setup_ioapic(void *ioapic_base, u8 ioapic_id)
{
	set_ioapic_id(ioapic_base, ioapic_id);
	clear_ioapic(ioapic_base);
	route_i8259_irq0(ioapic_base);
}
