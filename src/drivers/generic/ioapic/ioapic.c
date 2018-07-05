#include <console/console.h>
#include <device/device.h>
#include "chip.h"
#include <arch/ioapic.h>
#include <cpu/x86/lapic.h>

static void ioapic_init(struct device *dev)
{
	struct drivers_generic_ioapic_config *config = dev->chip_info;
	u32 bsp_lapicid = lapicid();
	u32 low, high;
	u32 i, ioapic_interrupts;
	void *ioapic_base;
	u8 ioapic_id;

	if (!dev->enabled || !config)
		return;

	ioapic_base = config->base;
	ioapic_id = config->apicid;

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

	/* Read the available number of interrupts. */
	ioapic_interrupts = (io_apic_read(ioapic_base, 0x01) >> 16) & 0xff;
	if (!ioapic_interrupts || ioapic_interrupts == 0xff)
		ioapic_interrupts = 24;
	printk(BIOS_DEBUG, "IOAPIC: %d interrupts\n", ioapic_interrupts);

	if (config->irq_on_fsb) {
		/*
		 * For the Pentium 4 and above APICs deliver their interrupts
		 * on the front side bus, enable that.
		 */
		printk(BIOS_DEBUG, "IOAPIC: Enabling interrupts on FSB\n");
		io_apic_write(ioapic_base, 0x03,
		      io_apic_read(ioapic_base, 0x03) | (1 << 0));
	} else {
		printk(BIOS_DEBUG, "IOAPIC: Enabling interrupts on APIC serial bus\n");
		io_apic_write(ioapic_base, 0x03, 0);
	}

	if (config->enable_virtual_wire) {
		/* Enable Virtual Wire Mode. */
		low = INT_ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
		high = bsp_lapicid << (56 - 32);

		io_apic_write(ioapic_base, 0x10, low);
		io_apic_write(ioapic_base, 0x11, high);

		if (io_apic_read(ioapic_base, 0x10) == 0xffffffff) {
			printk(BIOS_WARNING, "IOAPIC not responding.\n");
			return;
		}

		printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n", 0,
			high, low);
	}
	low = INT_DISABLED;
	high = NONE;

	for (i = 1; i < ioapic_interrupts; i++) {
		io_apic_write(ioapic_base, i * 2 + 0x10, low);
		io_apic_write(ioapic_base, i * 2 + 0x11, high);

		printk(BIOS_SPEW, "IOAPIC: reg 0x%08x value 0x%08x 0x%08x\n",
		       i, high, low);
	}
}

static void ioapic_read_resources(struct device *dev)
{
	struct drivers_generic_ioapic_config *config = (struct drivers_generic_ioapic_config *)dev->chip_info;
	struct resource *res;

	res = new_resource(dev, 0);
	res->base = (resource_t)(uintptr_t)config->base;
	res->size = 0x1000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations ioapic_operations = {
	.read_resources   = ioapic_read_resources,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = ioapic_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &ioapic_operations;
}

struct chip_operations drivers_generic_ioapic_ops = {
	CHIP_NAME("IOAPIC")
	.enable_dev = enable_dev,
};
