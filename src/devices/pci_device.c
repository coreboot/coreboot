/*
 * This file is part of the coreboot project.
 *
 * It was originally based on the Linux kernel (drivers/pci/pci.c).
 *
 * Modifications are:
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2003-2006 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2004-2005 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005-2006 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2005-2009 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 */

/*
 * PCI Bus Services, see include/linux/pci.h for further explanation.
 *
 * Copyright 1993 -- 1997 Drew Eckhardt, Frederic Potter,
 * David Mosberger-Tang
 *
 * Copyright 1997 -- 1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 */

#include <console/console.h>
#include <stdlib.h>
#include <stdint.h>
#include <bitops.h>
#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <delay.h>
#if CONFIG_HYPERTRANSPORT_PLUGIN_SUPPORT == 1
#include <device/hypertransport.h>
#endif
#if CONFIG_PCIX_PLUGIN_SUPPORT == 1
#include <device/pcix.h>
#endif
#if CONFIG_PCIEXP_PLUGIN_SUPPORT == 1
#include <device/pciexp.h>
#endif
#if CONFIG_AGP_PLUGIN_SUPPORT == 1
#include <device/agp.h>
#endif
#if CONFIG_CARDBUS_PLUGIN_SUPPORT == 1
#include <device/cardbus.h>
#endif
#define CONFIG_PC80_SYSTEM 1
#if CONFIG_PC80_SYSTEM == 1
#include <pc80/i8259.h>
#endif

u8 pci_moving_config8(struct device *dev, unsigned int reg)
{
	u8 value, ones, zeroes;

	value = pci_read_config8(dev, reg);

	pci_write_config8(dev, reg, 0xff);
	ones = pci_read_config8(dev, reg);

	pci_write_config8(dev, reg, 0x00);
	zeroes = pci_read_config8(dev, reg);

	pci_write_config8(dev, reg, value);

	return ones ^ zeroes;
}

u16 pci_moving_config16(struct device *dev, unsigned int reg)
{
	u16 value, ones, zeroes;

	value = pci_read_config16(dev, reg);

	pci_write_config16(dev, reg, 0xffff);
	ones = pci_read_config16(dev, reg);

	pci_write_config16(dev, reg, 0x0000);
	zeroes = pci_read_config16(dev, reg);

	pci_write_config16(dev, reg, value);

	return ones ^ zeroes;
}

u32 pci_moving_config32(struct device *dev, unsigned int reg)
{
	u32 value, ones, zeroes;

	value = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, 0xffffffff);
	ones = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, 0x00000000);
	zeroes = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, value);

	return ones ^ zeroes;
}

/**
 * Given a device, a capability type, and a last position, return the next
 * matching capability. Always start at the head of the list.
 *
 * @param dev Pointer to the device structure.
 * @param cap PCI_CAP_LIST_ID of the PCI capability we're looking for.
 * @param last Location of the PCI capability register to start from.
 * @return The next matching capability.
 */
unsigned pci_find_next_capability(struct device *dev, unsigned cap,
				  unsigned last)
{
	unsigned pos = 0;
	u16 status;
	unsigned reps = 48;

	status = pci_read_config16(dev, PCI_STATUS);
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	switch (dev->hdr_type & 0x7f) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		pos = PCI_CAPABILITY_LIST;
		break;
	case PCI_HEADER_TYPE_CARDBUS:
		pos = PCI_CB_CAPABILITY_LIST;
		break;
	default:
		return 0;
	}

	pos = pci_read_config8(dev, pos);
	while (reps-- && (pos >= 0x40)) { /* Loop through the linked list. */
		int this_cap;

		pos &= ~3;
		this_cap = pci_read_config8(dev, pos + PCI_CAP_LIST_ID);
		printk(BIOS_SPEW, "Capability: type 0x%02x @ 0x%02x\n",
		       this_cap, pos);
		if (this_cap == 0xff)
			break;

		if (!last && (this_cap == cap))
			return pos;

		if (last == pos)
			last = 0;

		pos = pci_read_config8(dev, pos + PCI_CAP_LIST_NEXT);
	}
	return 0;
}

/**
 * Given a device, and a capability type, return the next matching
 * capability. Always start at the head of the list.
 *
 * @param dev Pointer to the device structure.
 * @param cap PCI_CAP_LIST_ID of the PCI capability we're looking for.
 * @return The next matching capability.
 */
unsigned pci_find_capability(device_t dev, unsigned cap)
{
	return pci_find_next_capability(dev, cap, 0);
}

/**
 * Given a device and register, read the size of the BAR for that register.
 *
 * @param dev Pointer to the device structure.
 * @param index Address of the PCI configuration register.
 * @return TODO
 */
struct resource *pci_get_resource(struct device *dev, unsigned long index)
{
	struct resource *resource;
	unsigned long value, attr;
	resource_t moving, limit;

	/* Initialize the resources to nothing. */
	resource = new_resource(dev, index);

	/* Get the initial value. */
	value = pci_read_config32(dev, index);

	/* See which bits move. */
	moving = pci_moving_config32(dev, index);

	/* Initialize attr to the bits that do not move. */
	attr = value & ~moving;

	/* If it is a 64bit resource look at the high half as well. */
	if (((attr & PCI_BASE_ADDRESS_SPACE_IO) == 0) &&
	    ((attr & PCI_BASE_ADDRESS_MEM_LIMIT_MASK) ==
	     PCI_BASE_ADDRESS_MEM_LIMIT_64)) {
		/* Find the high bits that move. */
		moving |=
		    ((resource_t) pci_moving_config32(dev, index + 4)) << 32;
	}

	/* Find the resource constraints.
	 * Start by finding the bits that move. From there:
	 * - Size is the least significant bit of the bits that move.
	 * - Limit is all of the bits that move plus all of the lower bits.
	 * See PCI Spec 6.2.5.1.
	 */
	limit = 0;
	if (moving) {
		resource->size = 1;
		resource->align = resource->gran = 0;
		while (!(moving & resource->size)) {
			resource->size <<= 1;
			resource->align += 1;
			resource->gran += 1;
		}
		resource->limit = limit = moving | (resource->size - 1);
	}

	/*
	 * Some broken hardware has read-only registers that do not
	 * really size correctly.
	 *
	 * Example: the Acer M7229 has BARs 1-4 normally read-only,
	 * so BAR1 at offset 0x10 reads 0x1f1. If you size that register
	 * by writing 0xffffffff to it, it will read back as 0x1f1 -- which
	 * is a violation of the spec.
	 *
	 * We catch this case and ignore it by observing which bits move.
	 *
	 * This also catches the common case of unimplemented registers
	 * that always read back as 0.
	 */
	if (moving == 0) {
		if (value != 0) {
			printk(BIOS_DEBUG, "%s register %02lx(%08lx), "
			       "read-only ignoring it\n",
			       dev_path(dev), index, value);
		}
		resource->flags = 0;
	} else if (attr & PCI_BASE_ADDRESS_SPACE_IO) {
		/* An I/O mapped base address. */
		attr &= PCI_BASE_ADDRESS_IO_ATTR_MASK;
		resource->flags |= IORESOURCE_IO;
		/* I don't want to deal with 32bit I/O resources. */
		resource->limit = 0xffff;
	} else {
		/* A Memory mapped base address. */
		attr &= PCI_BASE_ADDRESS_MEM_ATTR_MASK;
		resource->flags |= IORESOURCE_MEM;
		if (attr & PCI_BASE_ADDRESS_MEM_PREFETCH)
			resource->flags |= IORESOURCE_PREFETCH;
		attr &= PCI_BASE_ADDRESS_MEM_LIMIT_MASK;
		if (attr == PCI_BASE_ADDRESS_MEM_LIMIT_32) {
			/* 32bit limit. */
			resource->limit = 0xffffffffUL;
		} else if (attr == PCI_BASE_ADDRESS_MEM_LIMIT_1M) {
			/* 1MB limit. */
			resource->limit = 0x000fffffUL;
		} else if (attr == PCI_BASE_ADDRESS_MEM_LIMIT_64) {
			/* 64bit limit. */
			resource->limit = 0xffffffffffffffffULL;
			resource->flags |= IORESOURCE_PCI64;
		} else {
			/* Invalid value. */
			printk(BIOS_ERR, "Broken BAR with value %lx\n", attr);
			printk(BIOS_ERR, " on dev %s at index %02lx\n",
			       dev_path(dev), index);
			resource->flags = 0;
		}
	}

	/* Don't let the limit exceed which bits can move. */
	if (resource->limit > limit)
		resource->limit = limit;

	return resource;
}

/**
 * Given a device and an index, read the size of the BAR for that register.
 *
 * @param dev Pointer to the device structure.
 * @param index Address of the PCI configuration register.
 */
static void pci_get_rom_resource(struct device *dev, unsigned long index)
{
	struct resource *resource;
	unsigned long value;
	resource_t moving;

	/* Initialize the resources to nothing. */
	resource = new_resource(dev, index);

	/* Get the initial value. */
	value = pci_read_config32(dev, index);

	/* See which bits move. */
	moving = pci_moving_config32(dev, index);

	/* Clear the Enable bit. */
	moving = moving & ~PCI_ROM_ADDRESS_ENABLE;

	/* Find the resource constraints.
	 * Start by finding the bits that move. From there:
	 * - Size is the least significant bit of the bits that move.
	 * - Limit is all of the bits that move plus all of the lower bits.
	 * See PCI Spec 6.2.5.1.
	 */
	if (moving) {
		resource->size = 1;
		resource->align = resource->gran = 0;
		while (!(moving & resource->size)) {
			resource->size <<= 1;
			resource->align += 1;
			resource->gran += 1;
		}
		resource->limit = moving | (resource->size - 1);
		resource->flags |= IORESOURCE_MEM | IORESOURCE_READONLY;
	} else {
		if (value != 0) {
			printk(BIOS_DEBUG, "%s register %02lx(%08lx), "
			       "read-only ignoring it\n",
			       dev_path(dev), index, value);
		}
		resource->flags = 0;
	}
	compact_resources(dev);
}

/**
 * Read the base address registers for a given device.
 *
 * @param dev Pointer to the dev structure.
 * @param howmany How many registers to read (6 for device, 2 for bridge).
 */
static void pci_read_bases(struct device *dev, unsigned int howmany)
{
	unsigned long index;

	for (index = PCI_BASE_ADDRESS_0;
	     (index < PCI_BASE_ADDRESS_0 + (howmany << 2));) {
		struct resource *resource;
		resource = pci_get_resource(dev, index);
		index += (resource->flags & IORESOURCE_PCI64) ? 8 : 4;
	}

	compact_resources(dev);
}

static void pci_record_bridge_resource(struct device *dev, resource_t moving,
				       unsigned index, unsigned long type)
{
	struct resource *resource;
	unsigned long gran;
	resource_t step;

	resource = NULL;

	if (!moving)
		return;

	/* Initialize the constraints on the current bus. */
	resource = new_resource(dev, index);
	resource->size = 0;
	gran = 0;
	step = 1;
	while ((moving & step) == 0) {
		gran += 1;
		step <<= 1;
	}
	resource->gran = gran;
	resource->align = gran;
	resource->limit = moving | (step - 1);
	resource->flags = type | IORESOURCE_PCI_BRIDGE |
			  IORESOURCE_BRIDGE;
}

static void pci_bridge_read_bases(struct device *dev)
{
	resource_t moving_base, moving_limit, moving;

	/* See if the bridge I/O resources are implemented. */
	moving_base = ((u32) pci_moving_config8(dev, PCI_IO_BASE)) << 8;
	moving_base |=
	  ((u32) pci_moving_config16(dev, PCI_IO_BASE_UPPER16)) << 16;

	moving_limit = ((u32) pci_moving_config8(dev, PCI_IO_LIMIT)) << 8;
	moving_limit |=
	  ((u32) pci_moving_config16(dev, PCI_IO_LIMIT_UPPER16)) << 16;

	moving = moving_base & moving_limit;

	/* Initialize the I/O space constraints on the current bus. */
	pci_record_bridge_resource(dev, moving, PCI_IO_BASE, IORESOURCE_IO);

	/* See if the bridge prefmem resources are implemented. */
	moving_base =
	  ((resource_t) pci_moving_config16(dev, PCI_PREF_MEMORY_BASE)) << 16;
	moving_base |=
	  ((resource_t) pci_moving_config32(dev, PCI_PREF_BASE_UPPER32)) << 32;

	moving_limit =
	  ((resource_t) pci_moving_config16(dev, PCI_PREF_MEMORY_LIMIT)) << 16;
	moving_limit |=
	  ((resource_t) pci_moving_config32(dev, PCI_PREF_LIMIT_UPPER32)) << 32;

	moving = moving_base & moving_limit;
	/* Initialize the prefetchable memory constraints on the current bus. */
	pci_record_bridge_resource(dev, moving, PCI_PREF_MEMORY_BASE,
				   IORESOURCE_MEM | IORESOURCE_PREFETCH);

	/* See if the bridge mem resources are implemented. */
	moving_base = ((u32) pci_moving_config16(dev, PCI_MEMORY_BASE)) << 16;
	moving_limit = ((u32) pci_moving_config16(dev, PCI_MEMORY_LIMIT)) << 16;

	moving = moving_base & moving_limit;

	/* Initialize the memory resources on the current bus. */
	pci_record_bridge_resource(dev, moving, PCI_MEMORY_BASE,
				   IORESOURCE_MEM);

	compact_resources(dev);
}

void pci_dev_read_resources(struct device *dev)
{
	pci_read_bases(dev, 6);
	pci_get_rom_resource(dev, PCI_ROM_ADDRESS);
}

void pci_bus_read_resources(struct device *dev)
{
	pci_bridge_read_bases(dev);
	pci_read_bases(dev, 2);
	pci_get_rom_resource(dev, PCI_ROM_ADDRESS1);
}

void pci_domain_read_resources(struct device *dev)
{
	struct resource *res;

	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->limit = 0xffffffffULL;
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED;
}

static void pci_set_resource(struct device *dev, struct resource *resource)
{
	resource_t base, end;

	/* Make certain the resource has actually been assigned a value. */
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		printk(BIOS_ERR, "ERROR: %s %02lx %s size: 0x%010llx not "
		       "assigned\n", dev_path(dev), resource->index,
		       resource_type(resource), resource->size);
		return;
	}

	/* If this resource is fixed don't worry about it. */
	if (resource->flags & IORESOURCE_FIXED)
		return;

	/* If I have already stored this resource don't worry about it. */
	if (resource->flags & IORESOURCE_STORED)
		return;

	/* If the resource is subtractive don't worry about it. */
	if (resource->flags & IORESOURCE_SUBTRACTIVE)
		return;

	/* Only handle PCI memory and I/O resources for now. */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Enable the resources in the command register. */
	if (resource->size) {
		if (resource->flags & IORESOURCE_MEM)
			dev->command |= PCI_COMMAND_MEMORY;
		if (resource->flags & IORESOURCE_IO)
			dev->command |= PCI_COMMAND_IO;
		if (resource->flags & IORESOURCE_PCI_BRIDGE)
			dev->command |= PCI_COMMAND_MASTER;
	}

	/* Get the base address. */
	base = resource->base;

	/* Get the end. */
	end = resource_end(resource);

	/* Now store the resource. */
	resource->flags |= IORESOURCE_STORED;

	/*
	 * PCI bridges have no enable bit. They are disabled if the base of
	 * the range is greater than the limit. If the size is zero, disable
	 * by setting the base = limit and end = limit - 2^gran.
	 */
	if (resource->size == 0 && (resource->flags & IORESOURCE_PCI_BRIDGE)) {
		base = resource->limit;
		end = resource->limit - (1 << resource->gran);
		resource->base = base;
	}

	if (!(resource->flags & IORESOURCE_PCI_BRIDGE)) {
		unsigned long base_lo, base_hi;

		/*
		 * Some chipsets allow us to set/clear the I/O bit
		 * (e.g. VIA 82C686A). So set it to be safe.
		 */
		base_lo = base & 0xffffffff;
		base_hi = (base >> 32) & 0xffffffff;
		if (resource->flags & IORESOURCE_IO)
			base_lo |= PCI_BASE_ADDRESS_SPACE_IO;
		pci_write_config32(dev, resource->index, base_lo);
		if (resource->flags & IORESOURCE_PCI64)
			pci_write_config32(dev, resource->index + 4, base_hi);
	} else if (resource->index == PCI_IO_BASE) {
		/* Set the I/O ranges. */
		pci_write_config8(dev, PCI_IO_BASE, base >> 8);
		pci_write_config16(dev, PCI_IO_BASE_UPPER16, base >> 16);
		pci_write_config8(dev, PCI_IO_LIMIT, end >> 8);
		pci_write_config16(dev, PCI_IO_LIMIT_UPPER16, end >> 16);
	} else if (resource->index == PCI_MEMORY_BASE) {
		/* Set the memory range. */
		pci_write_config16(dev, PCI_MEMORY_BASE, base >> 16);
		pci_write_config16(dev, PCI_MEMORY_LIMIT, end >> 16);
	} else if (resource->index == PCI_PREF_MEMORY_BASE) {
		/* Set the prefetchable memory range. */
		pci_write_config16(dev, PCI_PREF_MEMORY_BASE, base >> 16);
		pci_write_config32(dev, PCI_PREF_BASE_UPPER32, base >> 32);
		pci_write_config16(dev, PCI_PREF_MEMORY_LIMIT, end >> 16);
		pci_write_config32(dev, PCI_PREF_LIMIT_UPPER32, end >> 32);
	} else {
		/* Don't let me think I stored the resource. */
		resource->flags &= ~IORESOURCE_STORED;
		printk(BIOS_ERR, "ERROR: invalid resource->index %lx\n",
		       resource->index);
	}

	report_resource_stored(dev, resource, "");
}

void pci_dev_set_resources(struct device *dev)
{
	struct resource *res;
	struct bus *bus;
	u8 line;

	for (res = dev->resource_list; res; res = res->next)
		pci_set_resource(dev, res);

	for (bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children)
			assign_resources(bus);
	}

	/* Set a default latency timer. */
	pci_write_config8(dev, PCI_LATENCY_TIMER, 0x40);

	/* Set a default secondary latency timer. */
	if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE)
		pci_write_config8(dev, PCI_SEC_LATENCY_TIMER, 0x40);

	/* Zero the IRQ settings. */
	line = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (line)
		pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);

	/* Set the cache line size, so far 64 bytes is good for everyone. */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 64 >> 2);
}

void pci_dev_enable_resources(struct device *dev)
{
	const struct pci_operations *ops;
	u16 command;

	/* Set the subsystem vendor and device ID for mainboard devices. */
	ops = ops_pci(dev);
	if (dev->on_mainboard && ops && ops->set_subsystem) {
		printk(BIOS_DEBUG, "%s subsystem <- %02x/%02x\n", dev_path(dev),
		       CONFIG_MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID,
		       CONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID);
		ops->set_subsystem(dev,
				   CONFIG_MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID,
				   CONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID);
	}
	command = pci_read_config16(dev, PCI_COMMAND);
	command |= dev->command;

	/* v3 has
	 * command |= (PCI_COMMAND_PARITY + PCI_COMMAND_SERR);	// Error check.
	 */

	printk(BIOS_DEBUG, "%s cmd <- %02x\n", dev_path(dev), command);
	pci_write_config16(dev, PCI_COMMAND, command);
}

void pci_bus_enable_resources(struct device *dev)
{
	u16 ctrl;

	/*
	 * Enable I/O in command register if there is VGA card
	 * connected with (even it does not claim I/O resource).
	 */
	if (dev->link_list->bridge_ctrl & PCI_BRIDGE_CTL_VGA)
		dev->command |= PCI_COMMAND_IO;
	ctrl = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	ctrl |= dev->link_list->bridge_ctrl;
	ctrl |= (PCI_BRIDGE_CTL_PARITY + PCI_BRIDGE_CTL_SERR); /* Error check. */
	printk(BIOS_DEBUG, "%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, ctrl);

	pci_dev_enable_resources(dev);
}

void pci_bus_reset(struct bus *bus)
{
	u16 ctl;

	ctl = pci_read_config16(bus->dev, PCI_BRIDGE_CONTROL);
	ctl |= PCI_BRIDGE_CTL_BUS_RESET;
	pci_write_config16(bus->dev, PCI_BRIDGE_CONTROL, ctl);
	mdelay(10);

	ctl &= ~PCI_BRIDGE_CTL_BUS_RESET;
	pci_write_config16(bus->dev, PCI_BRIDGE_CONTROL, ctl);
	delay(1);
}

void pci_dev_set_subsystem(struct device *dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

/** Default handler: only runs the relevant PCI BIOS. */
void pci_dev_init(struct device *dev)
{
#if CONFIG_PCI_ROM_RUN == 1 || CONFIG_VGA_ROM_RUN == 1
	struct rom_header *rom, *ram;

	if (CONFIG_PCI_ROM_RUN != 1 && /* Only execute VGA ROMs. */
	    ((dev->class >> 8) != PCI_CLASS_DISPLAY_VGA))
		return;

	if (CONFIG_VGA_ROM_RUN != 1 && /* Only execute non-VGA ROMs. */
	    ((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA))
		return;

	rom = pci_rom_probe(dev);
	if (rom == NULL)
		return;

	ram = pci_rom_load(dev, rom);
	if (ram == NULL)
		return;

	run_bios(dev, (unsigned long)ram);

#if CONFIG_CONSOLE_VGA == 1
	if ((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA)
		vga_console_init();
#endif /* CONFIG_CONSOLE_VGA */
#endif /* CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN */
}

/** Default device operation for PCI devices */
static struct pci_operations pci_dev_ops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

struct device_operations default_pci_ops_dev = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.scan_bus         = 0,
	.enable           = 0,
	.ops_pci          = &pci_dev_ops_pci,
};

/** Default device operations for PCI bridges */
static struct pci_operations pci_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_pci_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = pci_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pci_bus_ops_pci,
};

/**
 * Detect the type of downstream bridge.
 *
 * This function is a heuristic to detect which type of bus is downstream
 * of a PCI-to-PCI bridge. This functions by looking for various capability
 * blocks to figure out the type of downstream bridge. PCI-X, PCI-E, and
 * Hypertransport all seem to have appropriate capabilities.
 *
 * When only a PCI-Express capability is found the type is examined to see
 * which type of bridge we have.
 *
 * @param dev Pointer to the device structure of the bridge.
 * @return Appropriate bridge operations.
 */
static struct device_operations *get_pci_bridge_ops(device_t dev)
{
	unsigned int pos;

#if CONFIG_PCIX_PLUGIN_SUPPORT == 1
	pos = pci_find_capability(dev, PCI_CAP_ID_PCIX);
	if (pos) {
		printk(BIOS_DEBUG, "%s subordinate bus PCI-X\n", dev_path(dev));
		return &default_pcix_ops_bus;
	}
#endif
#if CONFIG_AGP_PLUGIN_SUPPORT == 1
	/* How do I detect a PCI to AGP bridge? */
#endif
#if CONFIG_HYPERTRANSPORT_PLUGIN_SUPPORT == 1
	pos = 0;
	while ((pos = pci_find_next_capability(dev, PCI_CAP_ID_HT, pos))) {
		u16 flags;
		flags = pci_read_config16(dev, pos + PCI_CAP_FLAGS);
		if ((flags >> 13) == 1) {
			/* Host or Secondary Interface */
			printk(BIOS_DEBUG, "%s subordinate bus HT\n",
			       dev_path(dev));
			return &default_ht_ops_bus;
		}
	}
#endif
#if CONFIG_PCIEXP_PLUGIN_SUPPORT == 1
	pos = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (pos) {
		u16 flags;
		flags = pci_read_config16(dev, pos + PCI_EXP_FLAGS);
		switch ((flags & PCI_EXP_FLAGS_TYPE) >> 4) {
		case PCI_EXP_TYPE_ROOT_PORT:
		case PCI_EXP_TYPE_UPSTREAM:
		case PCI_EXP_TYPE_DOWNSTREAM:
			printk(BIOS_DEBUG, "%s subordinate bus PCI Express\n",
			       dev_path(dev));
			return &default_pciexp_ops_bus;
		case PCI_EXP_TYPE_PCI_BRIDGE:
			printk(BIOS_DEBUG, "%s subordinate PCI\n",
			       dev_path(dev));
			return &default_pci_ops_bus;
		default:
			break;
		}
	}
#endif
	return &default_pci_ops_bus;
}

/**
 * Set up PCI device operation.
 *
 * Check if it already has a driver. If not, use find_device_operations(),
 * or set to a default based on type.
 *
 * @param dev Pointer to the device whose pci_ops you want to set.
 * @see pci_drivers
 */
static void set_pci_ops(struct device *dev)
{
	struct pci_driver *driver;

	if (dev->ops)
		return;

	/*
	 * Look through the list of setup drivers and find one for
	 * this PCI device.
	 */
	for (driver = &pci_drivers[0]; driver != &epci_drivers[0]; driver++) {
		if ((driver->vendor == dev->vendor) &&
		    (driver->device == dev->device)) {
			dev->ops = (struct device_operations *)driver->ops;
			printk(BIOS_SPEW, "%s [%04x/%04x] %sops\n",
			       dev_path(dev), driver->vendor, driver->device,
			       (driver->ops->scan_bus ? "bus " : ""));
			return;
		}
	}

	/* If I don't have a specific driver use the default operations. */
	switch (dev->hdr_type & 0x7f) {	/* Header type */
	case PCI_HEADER_TYPE_NORMAL:
		if ((dev->class >> 8) == PCI_CLASS_BRIDGE_PCI)
			goto bad;
		dev->ops = &default_pci_ops_dev;
		break;
	case PCI_HEADER_TYPE_BRIDGE:
		if ((dev->class >> 8) != PCI_CLASS_BRIDGE_PCI)
			goto bad;
		dev->ops = get_pci_bridge_ops(dev);
		break;
#if CONFIG_CARDBUS_PLUGIN_SUPPORT == 1
	case PCI_HEADER_TYPE_CARDBUS:
		dev->ops = &default_cardbus_ops_bus;
		break;
#endif
default:
bad:
		if (dev->enabled) {
			printk(BIOS_ERR, "%s [%04x/%04x/%06x] has unknown "
			       "header type %02x, ignoring.\n", dev_path(dev),
			       dev->vendor, dev->device,
			       dev->class >> 8, dev->hdr_type);
		}
	}
}

/**
 * See if we have already allocated a device structure for a given devfn.
 *
 * Given a linked list of PCI device structures and a devfn number, find the
 * device structure correspond to the devfn, if present. This function also
 * removes the device structure from the linked list.
 *
 * @param list The device structure list.
 * @param devfn A device/function number.
 * @return Pointer to the device structure found or NULL if we have not
 *	   allocated a device for this devfn yet.
 */
static struct device *pci_scan_get_dev(struct device **list, unsigned int devfn)
{
	struct device *dev;

	dev = 0;
	for (; *list; list = &(*list)->sibling) {
		if ((*list)->path.type != DEVICE_PATH_PCI) {
			printk(BIOS_ERR, "child %s not a PCI device\n",
			       dev_path(*list));
			continue;
		}
		if ((*list)->path.pci.devfn == devfn) {
			/* Unlink from the list. */
			dev = *list;
			*list = (*list)->sibling;
			dev->sibling = NULL;
			break;
		}
	}

	/*
	 * Just like alloc_dev() add the device to the list of devices on the
	 * bus. When the list of devices was formed we removed all of the
	 * parents children, and now we are interleaving static and dynamic
	 * devices in order on the bus.
	 */
	if (dev) {
		struct device *child;

		/* Find the last child of our parent. */
		for (child = dev->bus->children; child && child->sibling;)
			child = child->sibling;

		/* Place the device on the list of children of its parent. */
		if (child)
			child->sibling = dev;
		else
			dev->bus->children = dev;
	}

	return dev;
}

/**
 * Scan a PCI bus.
 *
 * Determine the existence of a given PCI device. Allocate a new struct device
 * if dev==NULL was passed in and the device exists in hardware.
 *
 * @param dev Pointer to the dev structure.
 * @param bus Pointer to the bus structure.
 * @param devfn A device/function number to look at.
 * @return The device structure for the device (if found), NULL otherwise.
 */
device_t pci_probe_dev(device_t dev, struct bus *bus, unsigned devfn)
{
	u32 id, class;
	u8 hdr_type;

	/* Detect if a device is present. */
	if (!dev) {
		struct device dummy;

		dummy.bus = bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = devfn;

		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		/*
		 * Have we found something? Some broken boards return 0 if a
		 * slot is empty, but the expected answer is 0xffffffff.
		 */
		if (id == 0xffffffff)
			return NULL;

		if ((id == 0x00000000) || (id == 0x0000ffff) ||
		    (id == 0xffff0000)) {
			printk(BIOS_SPEW, "%s, bad id 0x%x\n",
			       dev_path(&dummy), id);
			return NULL;
		}
		dev = alloc_dev(bus, &dummy.path);
	} else {
		/*
		 * Enable/disable the device. Once we have found the device-
		 * specific operations this operations we will disable the
		 * device with those as well.
		 *
		 * This is geared toward devices that have subfunctions
		 * that do not show up by default.
		 *
		 * If a device is a stuff option on the motherboard
		 * it may be absent and enable_dev() must cope.
		 */
		/* Run the magic enable sequence for the device. */
		if (dev->chip_ops && dev->chip_ops->enable_dev)
			dev->chip_ops->enable_dev(dev);

		/* Now read the vendor and device ID. */
		id = pci_read_config32(dev, PCI_VENDOR_ID);

		/*
		 * If the device does not have a PCI ID disable it. Possibly
		 * this is because we have already disabled the device. But
		 * this also handles optional devices that may not always
		 * show up.
		 */
		/* If the chain is fully enumerated quit */
		if ((id == 0xffffffff) || (id == 0x00000000) ||
		    (id == 0x0000ffff) || (id == 0xffff0000)) {
			if (dev->enabled) {
				printk(BIOS_INFO, "PCI: Static device %s not "
				       "found, disabling it.\n", dev_path(dev));
				dev->enabled = 0;
			}
			return dev;
		}
	}

	/* Read the rest of the PCI configuration information. */
	hdr_type = pci_read_config8(dev, PCI_HEADER_TYPE);
	class = pci_read_config32(dev, PCI_CLASS_REVISION);

	/* Store the interesting information in the device structure. */
	dev->vendor = id & 0xffff;
	dev->device = (id >> 16) & 0xffff;
	dev->hdr_type = hdr_type;

	/* Class code, the upper 3 bytes of PCI_CLASS_REVISION. */
	dev->class = class >> 8;

	/* Architectural/System devices always need to be bus masters. */
	if ((dev->class >> 16) == PCI_BASE_CLASS_SYSTEM)
		dev->command |= PCI_COMMAND_MASTER;

	/*
	 * Look at the vendor and device ID, or at least the header type and
	 * class and figure out which set of configuration methods to use.
	 * Unless we already have some PCI ops.
	 */
	set_pci_ops(dev);

	/* Now run the magic enable/disable sequence for the device. */
	if (dev->ops && dev->ops->enable)
		dev->ops->enable(dev);

	/* Display the device. */
	printk(BIOS_DEBUG, "%s [%04x/%04x] %s%s\n", dev_path(dev),
	       dev->vendor, dev->device, dev->enabled ? "enabled" : "disabled",
	       dev->ops ? "" : " No operations");

	return dev;
}

/**
 * Scan a PCI bus.
 *
 * Determine the existence of devices and bridges on a PCI bus. If there are
 * bridges on the bus, recursively scan the buses behind the bridges.
 *
 * This function is the default scan_bus() method for the root device
 * 'dev_root'.
 *
 * @param bus Pointer to the bus structure.
 * @param min_devfn Minimum devfn to look at in the scan, usually 0x00.
 * @param max_devfn Maximum devfn to look at in the scan, usually 0xff.
 * @param max Current bus number.
 * @return The maximum bus number found, after scanning all subordinate busses.
 */
unsigned int pci_scan_bus(struct bus *bus, unsigned min_devfn,
			  unsigned max_devfn, unsigned int max)
{
	unsigned int devfn;
	struct device *old_devices;
	struct device *child;

#if CONFIG_PCI_BUS_SEGN_BITS
	printk(BIOS_DEBUG, "PCI: pci_scan_bus for bus %04x:%02x\n",
	       bus->secondary >> 8, bus->secondary & 0xff);
#else
	printk(BIOS_DEBUG, "PCI: pci_scan_bus for bus %02x\n", bus->secondary);
#endif

	/* Maximum sane devfn is 0xFF. */
	if (max_devfn > 0xff) {
		printk(BIOS_ERR, "PCI: pci_scan_bus limits devfn %x - "
		       "devfn %x\n", min_devfn, max_devfn);
		printk(BIOS_ERR, "PCI: pci_scan_bus upper limit too big. "
		       "Using 0xff.\n");
		max_devfn=0xff;
	}

	old_devices = bus->children;
	bus->children = NULL;

	post_code(0x24);

	/*
	 * Probe all devices/functions on this bus with some optimization for
	 * non-existence and single function devices.
	 */
	for (devfn = min_devfn; devfn <= max_devfn; devfn++) {
		struct device *dev;

		/* First thing setup the device structure. */
		dev = pci_scan_get_dev(&old_devices, devfn);

		/* See if a device is present and setup the device structure. */
		dev = pci_probe_dev(dev, bus, devfn);

		/*
		 * If this is not a multi function device, or the device is
		 * not present don't waste time probing another function.
		 * Skip to next device.
		 */
		if ((PCI_FUNC(devfn) == 0x00) && (!dev
		     || (dev->enabled && ((dev->hdr_type & 0x80) != 0x80)))) {
			devfn += 0x07;
		}
	}

	post_code(0x25);

	/*
	 * Warn if any leftover static devices are are found.
	 * There's probably a problem in devicetree.cb.
	 */
	if (old_devices) {
		device_t left;
		printk(BIOS_WARNING, "PCI: Left over static devices:\n");
		for (left = old_devices; left; left = left->sibling)
			printk(BIOS_WARNING, "%s\n", dev_path(left));

		printk(BIOS_WARNING, "PCI: Check your devicetree.cb.\n");
	}

	/*
	 * For all children that implement scan_bus() (i.e. bridges)
	 * scan the bus behind that child.
	 */
	for (child = bus->children; child; child = child->sibling)
		max = scan_bus(child, max);

	/*
	 * We've scanned the bus and so we know all about what's on the other
	 * side of any bridges that may be on this bus plus any devices.
	 * Return how far we've got finding sub-buses.
	 */
	printk(BIOS_DEBUG, "PCI: pci_scan_bus returning with max=%03x\n", max);
	post_code(0x55);
	return max;
}

/**
 * Scan a PCI bridge and the buses behind the bridge.
 *
 * Determine the existence of buses behind the bridge. Set up the bridge
 * according to the result of the scan.
 *
 * This function is the default scan_bus() method for PCI bridge devices.
 *
 * @param dev Pointer to the bridge device.
 * @param max The highest bus number assigned up to now.
 * @param do_scan_bus TODO
 * @return The maximum bus number found, after scanning all subordinate buses.
 */
unsigned int do_pci_scan_bridge(struct device *dev, unsigned int max,
				unsigned int (*do_scan_bus) (struct bus * bus,
							     unsigned min_devfn,
							     unsigned max_devfn,
							     unsigned int max))
{
	struct bus *bus;
	u32 buses;
	u16 cr;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(dev));

	if (dev->link_list == NULL) {
		struct bus *link;
		link = malloc(sizeof(*link));
		if (link == NULL)
			die("Couldn't allocate a link!\n");
		memset(link, 0, sizeof(*link));
		link->dev = dev;
		dev->link_list = link;
	}

	bus = dev->link_list;

	/*
	 * Set up the primary, secondary and subordinate bus numbers. We have
	 * no idea how many buses are behind this bridge yet, so we set the
	 * subordinate bus number to 0xff for the moment.
	 */
	bus->secondary = ++max;
	bus->subordinate = 0xff;

	/* Clear all status bits and turn off memory, I/O and master enables. */
	cr = pci_read_config16(dev, PCI_COMMAND);
	pci_write_config16(dev, PCI_COMMAND, 0x0000);
	pci_write_config16(dev, PCI_STATUS, 0xffff);

	/*
	 * Read the existing primary/secondary/subordinate bus
	 * number configuration.
	 */
	buses = pci_read_config32(dev, PCI_PRIMARY_BUS);

	/*
	 * Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagated by the bridge if it is not
	 * correctly configured.
	 */
	buses &= 0xff000000;
	buses |= (((unsigned int)(dev->bus->secondary) << 0) |
		  ((unsigned int)(bus->secondary) << 8) |
		  ((unsigned int)(bus->subordinate) << 16));
	pci_write_config32(dev, PCI_PRIMARY_BUS, buses);

	/* Now we can scan all subordinate buses (those behind the bridge). */
	max = do_scan_bus(bus, 0x00, 0xff, max);

	/*
	 * We know the number of buses behind this bridge. Set the subordinate
	 * bus number to its real value.
	 */
	bus->subordinate = max;
	buses = (buses & 0xff00ffff) | ((unsigned int)(bus->subordinate) << 16);
	pci_write_config32(dev, PCI_PRIMARY_BUS, buses);
	pci_write_config16(dev, PCI_COMMAND, cr);

	printk(BIOS_SPEW, "%s returns max %d\n", __func__, max);
	return max;
}

/**
 * Scan a PCI bridge and the buses behind the bridge.
 *
 * Determine the existence of buses behind the bridge. Set up the bridge
 * according to the result of the scan.
 *
 * This function is the default scan_bus() method for PCI bridge devices.
 *
 * @param dev Pointer to the bridge device.
 * @param max The highest bus number assigned up to now.
 * @return The maximum bus number found, after scanning all subordinate buses.
 */
unsigned int pci_scan_bridge(struct device *dev, unsigned int max)
{
	return do_pci_scan_bridge(dev, max, pci_scan_bus);
}

/**
 * Scan a PCI domain.
 *
 * This function is the default scan_bus() method for PCI domains.
 *
 * @param dev Pointer to the domain.
 * @param max The highest bus number assigned up to now.
 * @return The maximum bus number found, after scanning all subordinate busses.
 */
unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
	max = pci_scan_bus(dev->link_list, PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

#if CONFIG_PC80_SYSTEM == 1
/**
 * Assign IRQ numbers.
 *
 * This function assigns IRQs for all functions contained within the indicated
 * device address. If the device does not exist or does not require interrupts
 * then this function has no effect.
 *
 * This function should be called for each PCI slot in your system.
 *
 * @param bus Pointer to the bus structure.
 * @param slot TODO
 * @param pIntAtoD An array of IRQ #s that are assigned to PINTA through PINTD
 *        of this slot. The particular IRQ #s that are passed in depend on the
 *        routing inside your southbridge and on your board.
 */
void pci_assign_irqs(unsigned bus, unsigned slot,
		     const unsigned char pIntAtoD[4])
{
	unsigned int funct;
	device_t pdev;
	u8 line, irq;

	/* Each slot may contain up to eight functions. */
	for (funct = 0; funct < 8; funct++) {
		pdev = dev_find_slot(bus, (slot << 3) + funct);

		if (!pdev)
			continue;

		line = pci_read_config8(pdev, PCI_INTERRUPT_PIN);

		/* PCI spec says all values except 1..4 are reserved. */
		if ((line < 1) || (line > 4))
			continue;

		irq = pIntAtoD[line - 1];

		printk(BIOS_DEBUG, "Assigning IRQ %d to %d:%x.%d\n",
		       irq, bus, slot, funct);

		pci_write_config8(pdev, PCI_INTERRUPT_LINE,
				  pIntAtoD[line - 1]);

#ifdef PARANOID_IRQ_ASSIGNMENTS
		irq = pci_read_config8(pdev, PCI_INTERRUPT_LINE);
		printk(BIOS_DEBUG, "  Readback = %d\n", irq);
#endif

		/* Change to level triggered. */
		i8259_configure_irq_trigger(pIntAtoD[line - 1],
					    IRQ_LEVEL_TRIGGERED);
	}
}
#endif
