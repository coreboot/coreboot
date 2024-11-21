/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/device.h>
#include <device/mmio.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <inttypes.h>
#include <types.h>

#define ALL		(0xff << 24)
#define NONE		(0)
#define INT_DISABLED	(1 << 16)
#define INT_ENABLED	(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)

static u32 io_apic_read(uintptr_t ioapic_base, u32 reg)
{
	write32p(ioapic_base, reg);
	return read32p(ioapic_base + 0x10);
}

static void io_apic_write(uintptr_t ioapic_base, u32 reg, u32 value)
{
	write32p(ioapic_base, reg);
	write32p(ioapic_base + 0x10, value);
}

static void write_vector(uintptr_t ioapic_base, u8 vector, u32 high, u32 low)
{
	io_apic_write(ioapic_base, vector * 2 + 0x10, low);
	io_apic_write(ioapic_base, vector * 2 + 0x11, high);

	printk(BIOS_SPEW, "IOAPIC: vector 0x%02x value 0x%08x 0x%08x\n",
	       vector, high, low);
}

/* Bits 23-16 of register 0x01 specify the maximum redirection entry, which
 * is the number of interrupts minus 1. */
unsigned int ioapic_get_max_vectors(uintptr_t ioapic_base)
{
	u32 reg;
	u8 count;

	reg = io_apic_read(ioapic_base, 0x01);
	count = (reg >> 16) & 0xff;

	if (count == 0xff)
		count = 23;
	count++;

	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", count);
	return count;
}

/* Set maximum number of redirection entries (MRE). It is write-once register
 * for some chipsets, and a negative mre_count will lock it to the number
 * of vectors read from the register. */
void ioapic_set_max_vectors(uintptr_t ioapic_base, int mre_count)
{
	u32 reg;
	u8 count;

	reg = io_apic_read(ioapic_base, 0x01);
	count = (reg >> 16) & 0xff;
	if (mre_count > 0)
		count = mre_count - 1;
	reg &= ~(0xff << 16);
	reg |= count << 16;
	io_apic_write(ioapic_base, 0x01, reg);
}

void ioapic_lock_max_vectors(uintptr_t ioapic_base)
{
	ioapic_set_max_vectors(ioapic_base, -1);
}

static void clear_vectors(uintptr_t ioapic_base, u8 first, u8 last)
{
	u32 low, high;
	u8 i;

	printk(BIOS_DEBUG, "IOAPIC: Clearing IOAPIC at %" PRIxPTR "\n", ioapic_base);

	low = INT_DISABLED;
	high = NONE;

	for (i = first; i <= last; i++)
		write_vector(ioapic_base, i, high, low);

	if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
		printk(BIOS_WARNING, "IOAPIC not responding.\n");
		return;
	}
}

static void route_i8259_irq0(uintptr_t ioapic_base)
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

static void set_ioapic_id(uintptr_t ioapic_base, u8 ioapic_id)
{
	int i;

	printk(BIOS_DEBUG, "IOAPIC: Initializing IOAPIC at %" PRIxPTR "\n",
	       ioapic_base);
	printk(BIOS_DEBUG, "IOAPIC: ID = 0x%02x\n", ioapic_id);

	io_apic_write(ioapic_base, 0x00,
		      (io_apic_read(ioapic_base, 0x00) & 0xf0ffffff) | (ioapic_id << 24));

	printk(BIOS_SPEW, "IOAPIC: Dumping registers\n");
	for (i = 0; i < 3; i++)
		printk(BIOS_SPEW, "  reg 0x%04x: 0x%08x\n", i,
		       io_apic_read(ioapic_base, i));
}

u8 get_ioapic_id(uintptr_t ioapic_base)
{
	/*
	 * According to 82093AA I/O ADVANCED PROGRAMMABLE INTERRUPT CONTROLLER (IOAPIC)
	 * only 4 bits (24:27) are used for the ID. In practice the upper bits are either
	 * always 0 or used for larger IDs.
	 */
	return (io_apic_read(ioapic_base, 0x00) >> 24) & 0xff;
}

u8 get_ioapic_version(uintptr_t ioapic_base)
{
	return io_apic_read(ioapic_base, 0x01) & 0xff;
}

void ioapic_set_boot_config(uintptr_t ioapic_base, bool irq_on_fsb)
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

/**
 * Create a new IOAPIC device under the given device.
 *
 * @param parent      The parent device. A PCI domain or PCI device (in case of PCI IOAPIC).
 * @param ioapic_base The IOAPIC base address
 * @param gsi_base    Platform specific GSI base
 * @return Pointer to the device struct.
 */
struct device *ioapic_create_dev(struct device *parent,
				 const uintptr_t ioapic_base,
				 const u32 gsi_base)
{
	struct device_path path = {0};
	struct device *dev;

	if (!parent)
		return NULL;

	struct bus *bus = alloc_bus(parent);
	if (!bus)
		return NULL;

	if (gsi_base == 0)
		register_new_ioapic_gsi0(ioapic_base);
	else
		register_new_ioapic(ioapic_base);

	path.type = DEVICE_PATH_IOAPIC;
	path.ioapic.ioapic_id = get_ioapic_id(ioapic_base);
	path.ioapic.addr = ioapic_base;
	path.ioapic.gsi_base = gsi_base;

	dev = alloc_dev(bus, &path);
	assert(dev);

	return dev;
}

void setup_ioapic(uintptr_t ioapic_base, u8 ioapic_id)
{
	set_ioapic_id(ioapic_base, ioapic_id);
	clear_vectors(ioapic_base, 0, ioapic_get_max_vectors(ioapic_base) - 1);
	route_i8259_irq0(ioapic_base);
}

void register_new_ioapic_gsi0(uintptr_t ioapic_base)
{
	setup_ioapic(ioapic_base, 0);
}

void register_new_ioapic(uintptr_t ioapic_base)
{
	static u8 ioapic_id;
	ioapic_id++;
	set_ioapic_id(ioapic_base, ioapic_id);
	clear_vectors(ioapic_base, 0, ioapic_get_max_vectors(ioapic_base) - 1);
}
