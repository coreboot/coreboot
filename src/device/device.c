/*
 * This file is part of the coreboot project.
 *
 * It was originally based on the Linux kernel (arch/i386/kernel/pci-pc.c).
 *
 * Modifications are:
 * Copyright (C) 2003 Eric Biederman <ebiederm@xmission.com>
 * Copyright (C) 2003-2004 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2003 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2004-2005 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005-2006 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2005-2006 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2009 Myles Watson <mylesgw@gmail.com>
 * Copyright (c) 1999--2000 Martin Mares <mj@suse.cz>
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

/*
 * Lots of mods by Ron Minnich <rminnich@lanl.gov>, with
 * the final architecture guidance from Tom Merritt <tjm@codegen.com>.
 *
 * In particular, we changed from the one-pass original version to
 * Tom's recommended multiple-pass version. I wasn't sure about doing
 * it with multiple passes, until I actually started doing it and saw
 * the wisdom of Tom's recommendations...
 *
 * Lots of cleanups by Eric Biederman to handle bridges, and to
 * handle resource allocation for non-PCI devices.
 */

#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <smp/spinlock.h>
#if IS_ENABLED(CONFIG_ARCH_X86)
#include <arch/ebda.h>
#endif
#include <timer.h>

/** Linked list of ALL devices */
struct device *all_devices = &dev_root;
/** Pointer to the last device */
extern struct device *last_dev;
/** Linked list of free resources */
struct resource *free_resources = NULL;

/**
 * Initialize all chips of statically known devices.
 *
 * Will be called before bus enumeration to initialize chips stated in the
 * device tree.
 */
void dev_initialize_chips(void)
{
	const struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		/* Initialize chip if we haven't yet. */
		if (dev->chip_ops && dev->chip_ops->init &&
				!dev->chip_ops->initialized) {
			post_log_path(dev);
			dev->chip_ops->init(dev->chip_info);
			dev->chip_ops->initialized = 1;
		}
	}
	post_log_clear();
}

/**
 * Finalize all chips of statically known devices.
 *
 * This is the last call before calling the payload. This is a good place
 * to lock registers or other final cleanup.
 */
void dev_finalize_chips(void)
{
	const struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		/* Initialize chip if we haven't yet. */
		if (dev->chip_ops && dev->chip_ops->final &&
				!dev->chip_ops->finalized) {
			dev->chip_ops->final(dev->chip_info);
			dev->chip_ops->finalized = 1;
		}
	}
}

DECLARE_SPIN_LOCK(dev_lock)

#if IS_ENABLED(CONFIG_GFXUMA)
/* IGD UMA memory */
uint64_t uma_memory_base = 0;
uint64_t uma_memory_size = 0;
#endif

/**
 * Allocate a new device structure.
 *
 * Allocate a new device structure and attach it to the device tree as a
 * child of the parent bus.
 *
 * @param parent Parent bus the newly created device should be attached to.
 * @param path Path to the device to be created.
 * @return Pointer to the newly created device structure.
 *
 * @see device_path
 */
static device_t __alloc_dev(struct bus *parent, struct device_path *path)
{
	device_t dev, child;

	/* Find the last child of our parent. */
	for (child = parent->children; child && child->sibling; /* */ )
		child = child->sibling;

	dev = malloc(sizeof(*dev));
	if (dev == 0)
		die("alloc_dev(): out of memory.\n");

	memset(dev, 0, sizeof(*dev));
	memcpy(&dev->path, path, sizeof(*path));

	/* By default devices are enabled. */
	dev->enabled = 1;

	/* Add the new device to the list of children of the bus. */
	dev->bus = parent;
	if (child)
		child->sibling = dev;
	else
		parent->children = dev;

	/* Append a new device to the global device list.
	 * The list is used to find devices once everything is set up.
	 */
	last_dev->next = dev;
	last_dev = dev;

	return dev;
}

device_t alloc_dev(struct bus *parent, struct device_path *path)
{
	device_t dev;
	spin_lock(&dev_lock);
	dev = __alloc_dev(parent, path);
	spin_unlock(&dev_lock);
	return dev;
}

/**
 * See if a device structure already exists and if not allocate it.
 *
 * @param parent The bus to find the device on.
 * @param path The relative path from the bus to the appropriate device.
 * @return Pointer to a device structure for the device on bus at path.
 */
device_t alloc_find_dev(struct bus *parent, struct device_path *path)
{
	device_t child;
	spin_lock(&dev_lock);
	child = find_dev_path(parent, path);
	if (!child)
		child = __alloc_dev(parent, path);
	spin_unlock(&dev_lock);
	return child;
}

/**
 * Round a number up to an alignment.
 *
 * @param val The starting value.
 * @param pow Alignment as a power of two.
 * @return Rounded up number.
 */
static resource_t round(resource_t val, unsigned long pow)
{
	resource_t mask;
	mask = (1ULL << pow) - 1ULL;
	val += mask;
	val &= ~mask;
	return val;
}

static const char * resource2str(struct resource *res)
{
	if (res->flags & IORESOURCE_IO)
		return "io";
	if (res->flags & IORESOURCE_PREFETCH)
		return "prefmem";
	if (res->flags & IORESOURCE_MEM)
		return "mem";
	return "undefined";
}

/**
 * Read the resources on all devices of a given bus.
 *
 * @param bus Bus to read the resources on.
 */
static void read_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s %s bus %x link: %d\n", dev_path(bus->dev),
	       __func__, bus->secondary, bus->link_num);

	/* Walk through all devices and find which resources they need. */
	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		struct bus *link;

		if (!curdev->enabled)
			continue;

		if (!curdev->ops || !curdev->ops->read_resources) {
			if (curdev->path.type != DEVICE_PATH_APIC)
				printk(BIOS_ERR, "%s missing read_resources\n",
				       dev_path(curdev));
			continue;
		}
		post_log_path(curdev);
		curdev->ops->read_resources(curdev);

		/* Read in the resources behind the current device's links. */
		for (link = curdev->link_list; link; link = link->next)
			read_resources(link);
	}
	post_log_clear();
	printk(BIOS_SPEW, "%s read_resources bus %d link: %d done\n",
	       dev_path(bus->dev), bus->secondary, bus->link_num);
}

struct pick_largest_state {
	struct resource *last;
	const struct device *result_dev;
	struct resource *result;
	int seen_last;
};

static void pick_largest_resource(void *gp, struct device *dev,
				  struct resource *resource)
{
	struct pick_largest_state *state = gp;
	struct resource *last;

	last = state->last;

	/* Be certain to pick the successor to last. */
	if (resource == last) {
		state->seen_last = 1;
		return;
	}
	if (resource->flags & IORESOURCE_FIXED)
		return;	/* Skip it. */
	if (last && ((last->align < resource->align) ||
		     ((last->align == resource->align) &&
		      (last->size < resource->size)) ||
		     ((last->align == resource->align) &&
		      (last->size == resource->size) && (!state->seen_last)))) {
		return;
	}
	if (!state->result ||
	    (state->result->align < resource->align) ||
	    ((state->result->align == resource->align) &&
	     (state->result->size < resource->size))) {
		state->result_dev = dev;
		state->result = resource;
	}
}

static const struct device *largest_resource(struct bus *bus,
				       struct resource **result_res,
				       unsigned long type_mask,
				       unsigned long type)
{
	struct pick_largest_state state;

	state.last = *result_res;
	state.result_dev = NULL;
	state.result = NULL;
	state.seen_last = 0;

	search_bus_resources(bus, type_mask, type, pick_largest_resource,
			     &state);

	*result_res = state.result;
	return state.result_dev;
}

/**
 * This function is the guts of the resource allocator.
 *
 * The problem.
 *  - Allocate resource locations for every device.
 *  - Don't overlap, and follow the rules of bridges.
 *  - Don't overlap with resources in fixed locations.
 *  - Be efficient so we don't have ugly strategies.
 *
 * The strategy.
 * - Devices that have fixed addresses are the minority so don't
 *   worry about them too much. Instead only use part of the address
 *   space for devices with programmable addresses. This easily handles
 *   everything except bridges.
 *
 * - PCI devices are required to have their sizes and their alignments
 *   equal. In this case an optimal solution to the packing problem
 *   exists. Allocate all devices from highest alignment to least
 *   alignment or vice versa. Use this.
 *
 * - So we can handle more than PCI run two allocation passes on bridges. The
 *   first to see how large the resources are behind the bridge, and what
 *   their alignment requirements are. The second to assign a safe address to
 *   the devices behind the bridge. This allows us to treat a bridge as just
 *   a device with a couple of resources, and not need to special case it in
 *   the allocator. Also this allows handling of other types of bridges.
 *
 * @param bus The bus we are traversing.
 * @param bridge The bridge resource which must contain the bus' resources.
 * @param type_mask This value gets ANDed with the resource type.
 * @param type This value must match the result of the AND.
 * @return TODO
 */
static void compute_resources(struct bus *bus, struct resource *bridge,
			      unsigned long type_mask, unsigned long type)
{
	const struct device *dev;
	struct resource *resource;
	resource_t base;
	base = round(bridge->base, bridge->align);

	printk(BIOS_SPEW,  "%s %s: base: %llx size: %llx align: %d gran: %d"
	       " limit: %llx\n", dev_path(bus->dev), resource2str(bridge),
	       base, bridge->size, bridge->align,
	       bridge->gran, bridge->limit);

	/* For each child which is a bridge, compute the resource needs. */
	for (dev = bus->children; dev; dev = dev->sibling) {
		struct resource *child_bridge;

		if (!dev->link_list)
			continue;

		/* Find the resources with matching type flags. */
		for (child_bridge = dev->resource_list; child_bridge;
		     child_bridge = child_bridge->next) {
			struct bus* link;

			if (!(child_bridge->flags & IORESOURCE_BRIDGE)
			    || (child_bridge->flags & type_mask) != type)
				continue;

			/*
			 * Split prefetchable memory if combined. Many domains
			 * use the same address space for prefetchable memory
			 * and non-prefetchable memory. Bridges below them need
			 * it separated. Add the PREFETCH flag to the type_mask
			 * and type.
			 */
			link = dev->link_list;
			while (link && link->link_num !=
					IOINDEX_LINK(child_bridge->index))
				link = link->next;

			if (link == NULL) {
				printk(BIOS_ERR, "link %ld not found on %s\n",
				       IOINDEX_LINK(child_bridge->index),
				       dev_path(dev));
			}

			compute_resources(link, child_bridge,
					  type_mask | IORESOURCE_PREFETCH,
					  type | (child_bridge->flags &
						  IORESOURCE_PREFETCH));
		}
	}

	/* Remember we haven't found anything yet. */
	resource = NULL;

	/*
	 * Walk through all the resources on the current bus and compute the
	 * amount of address space taken by them. Take granularity and
	 * alignment into account.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {

		/* Size 0 resources can be skipped. */
		if (!resource->size)
			continue;

		/* Propagate the resource alignment to the bridge resource. */
		if (resource->align > bridge->align)
			bridge->align = resource->align;

		/* Propagate the resource limit to the bridge register. */
		if (bridge->limit > resource->limit)
			bridge->limit = resource->limit;

		/* Warn if it looks like APICs aren't declared. */
		if ((resource->limit == 0xffffffff) &&
		    (resource->flags & IORESOURCE_ASSIGNED)) {
			printk(BIOS_ERR,
			       "Resource limit looks wrong! (no APIC?)\n");
			printk(BIOS_ERR, "%s %02lx limit %08llx\n",
			       dev_path(dev), resource->index, resource->limit);
		}

		if (resource->flags & IORESOURCE_IO) {
			/*
			 * Don't allow potential aliases over the legacy PCI
			 * expansion card addresses. The legacy PCI decodes
			 * only 10 bits, uses 0x100 - 0x3ff. Therefore, only
			 * 0x00 - 0xff can be used out of each 0x400 block of
			 * I/O space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/*
			 * Don't allow allocations in the VGA I/O range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}
		/* Base must be aligned. */
		base = round(base, resource->align);
		resource->base = base;
		base += resource->size;

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->base + resource->size - 1,
		       resource2str(resource));
	}

	/*
	 * A PCI bridge resource does not need to be a power of two size, but
	 * it does have a minimum granularity. Round the size up to that
	 * minimum granularity so we know not to place something else at an
	 * address positively decoded by the bridge.
	 */
	bridge->size = round(base, bridge->gran) -
		       round(bridge->base, bridge->align);

	printk(BIOS_SPEW, "%s %s: base: %llx size: %llx align: %d gran: %d"
	       " limit: %llx done\n", dev_path(bus->dev),
	       resource2str(bridge),
	       base, bridge->size, bridge->align, bridge->gran, bridge->limit);
}

/**
 * This function is the second part of the resource allocator.
 *
 * See the compute_resources function for a more detailed explanation.
 *
 * This function assigns the resources a value.
 *
 * @param bus The bus we are traversing.
 * @param bridge The bridge resource which must contain the bus' resources.
 * @param type_mask This value gets ANDed with the resource type.
 * @param type This value must match the result of the AND.
 *
 * @see compute_resources
 */
static void allocate_resources(struct bus *bus, struct resource *bridge,
			       unsigned long type_mask, unsigned long type)
{
	const struct device *dev;
	struct resource *resource;
	resource_t base;
	base = bridge->base;

	printk(BIOS_SPEW, "%s %s: base:%llx size:%llx align:%d gran:%d "
	       "limit:%llx\n", dev_path(bus->dev),
	       resource2str(bridge),
	       base, bridge->size, bridge->align, bridge->gran, bridge->limit);

	/* Remember we haven't found anything yet. */
	resource = NULL;

	/*
	 * Walk through all the resources on the current bus and allocate them
	 * address space.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {

		/* Propagate the bridge limit to the resource register. */
		if (resource->limit > bridge->limit)
			resource->limit = bridge->limit;

		/* Size 0 resources can be skipped. */
		if (!resource->size) {
			/* Set the base to limit so it doesn't confuse tolm. */
			resource->base = resource->limit;
			resource->flags |= IORESOURCE_ASSIGNED;
			continue;
		}

		if (resource->flags & IORESOURCE_IO) {
			/*
			 * Don't allow potential aliases over the legacy PCI
			 * expansion card addresses. The legacy PCI decodes
			 * only 10 bits, uses 0x100 - 0x3ff. Therefore, only
			 * 0x00 - 0xff can be used out of each 0x400 block of
			 * I/O space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/*
			 * Don't allow allocations in the VGA I/O range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}

		if ((round(base, resource->align) + resource->size - 1) <=
		    resource->limit) {
			/* Base must be aligned. */
			base = round(base, resource->align);
			resource->base = base;
			resource->limit = resource->base + resource->size - 1;
			resource->flags |= IORESOURCE_ASSIGNED;
			resource->flags &= ~IORESOURCE_STORED;
			base += resource->size;
		} else {
			printk(BIOS_ERR, "!! Resource didn't fit !!\n");
			printk(BIOS_ERR, "   aligned base %llx size %llx "
			       "limit %llx\n", round(base, resource->align),
			       resource->size, resource->limit);
			printk(BIOS_ERR, "   %llx needs to be <= %llx "
			       "(limit)\n", (round(base, resource->align) +
				resource->size) - 1, resource->limit);
			printk(BIOS_ERR, "   %s%s %02lx *  [0x%llx - 0x%llx]"
			       " %s\n", (resource->flags & IORESOURCE_ASSIGNED)
			       ? "Assigned: " : "", dev_path(dev),
			       resource->index, resource->base,
			       resource->base + resource->size - 1,
			       resource2str(resource));
		}

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->size ? resource->base + resource->size - 1 :
		       resource->base, resource2str(resource));
	}

	/*
	 * A PCI bridge resource does not need to be a power of two size, but
	 * it does have a minimum granularity. Round the size up to that
	 * minimum granularity so we know not to place something else at an
	 * address positively decoded by the bridge.
	 */

	bridge->flags |= IORESOURCE_ASSIGNED;

	printk(BIOS_SPEW, "%s %s: next_base: %llx size: %llx align: %d "
	       "gran: %d done\n", dev_path(bus->dev),
	       resource2str(bridge), base, bridge->size, bridge->align,
	       bridge->gran);

	/* For each child which is a bridge, allocate_resources. */
	for (dev = bus->children; dev; dev = dev->sibling) {
		struct resource *child_bridge;

		if (!dev->link_list)
			continue;

		/* Find the resources with matching type flags. */
		for (child_bridge = dev->resource_list; child_bridge;
		     child_bridge = child_bridge->next) {
			struct bus* link;

			if (!(child_bridge->flags & IORESOURCE_BRIDGE) ||
			    (child_bridge->flags & type_mask) != type)
				continue;

			/*
			 * Split prefetchable memory if combined. Many domains
			 * use the same address space for prefetchable memory
			 * and non-prefetchable memory. Bridges below them need
			 * it separated. Add the PREFETCH flag to the type_mask
			 * and type.
			 */
			link = dev->link_list;
			while (link && link->link_num !=
			               IOINDEX_LINK(child_bridge->index))
				link = link->next;
			if (link == NULL)
				printk(BIOS_ERR, "link %ld not found on %s\n",
				       IOINDEX_LINK(child_bridge->index),
				       dev_path(dev));

			allocate_resources(link, child_bridge,
					   type_mask | IORESOURCE_PREFETCH,
					   type | (child_bridge->flags &
						   IORESOURCE_PREFETCH));
		}
	}
}

static int resource_is(struct resource *res, u32 type)
{
	return (res->flags & IORESOURCE_TYPE_MASK) == type;
}

struct constraints {
	struct resource io, mem;
};

static struct resource * resource_limit(struct constraints *limits, struct resource *res)
{
	struct resource *lim = NULL;

	/* MEM, or I/O - skip any others. */
	if (resource_is(res, IORESOURCE_MEM))
		lim = &limits->mem;
	else if (resource_is(res, IORESOURCE_IO))
		lim = &limits->io;

	return lim;
}

static void constrain_resources(const struct device *dev, struct constraints* limits)
{
	const struct device *child;
	struct resource *res;
	struct resource *lim;
	struct bus *link;

	/* Constrain limits based on the fixed resources of this device. */
	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_FIXED))
			continue;
		if (!res->size) {
			/* It makes no sense to have 0-sized, fixed resources.*/
			printk(BIOS_ERR, "skipping %s@%lx fixed resource, "
			       "size=0!\n", dev_path(dev), res->index);
			continue;
		}

		lim = resource_limit(limits, res);
		if (!lim)
			continue;

		/*
		 * Is it a fixed resource outside the current known region?
		 * If so, we don't have to consider it - it will be handled
		 * correctly and doesn't affect current region's limits.
		 */
		if (((res->base + res->size -1) < lim->base)
		    || (res->base > lim->limit))
			continue;

		printk(BIOS_SPEW, "%s: %s %02lx base %08llx limit %08llx %s (fixed)\n",
			__func__, dev_path(dev), res->index, res->base,
			res->base + res->size - 1, resource2str(res));

		/*
		 * Choose to be above or below fixed resources. This check is
		 * signed so that "negative" amounts of space are handled
		 * correctly.
		 */
		if ((signed long long)(lim->limit - (res->base + res->size -1))
		    > (signed long long)(res->base - lim->base))
			lim->base = res->base + res->size;
		else
			lim->limit = res->base -1;
	}

	/* Descend into every enabled child and look for fixed resources. */
	for (link = dev->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling) {
			if (child->enabled)
				constrain_resources(child, limits);
		}
	}
}

static void avoid_fixed_resources(const struct device *dev)
{
	struct constraints limits;
	struct resource *res;
	struct resource *lim;

	printk(BIOS_SPEW, "%s: %s\n", __func__, dev_path(dev));

	/* Initialize constraints to maximum size. */
	limits.io.base = 0;
	limits.io.limit = 0xffffffffffffffffULL;
	limits.mem.base = 0;
	limits.mem.limit = 0xffffffffffffffffULL;

	/* Constrain the limits to dev's initial resources. */
	for (res = dev->resource_list; res; res = res->next) {
		if ((res->flags & IORESOURCE_FIXED))
			continue;
		printk(BIOS_SPEW, "%s:@%s %02lx limit %08llx\n", __func__,
		       dev_path(dev), res->index, res->limit);

		lim = resource_limit(&limits, res);
		if (!lim)
			continue;

		if (res->base > lim->base)
			lim->base = res->base;
		if  (res->limit < lim->limit)
			lim->limit = res->limit;
	}

	/* Look through the tree for fixed resources and update the limits. */
	constrain_resources(dev, &limits);

	/* Update dev's resources with new limits. */
	for (res = dev->resource_list; res; res = res->next) {
		if ((res->flags & IORESOURCE_FIXED))
			continue;

		lim = resource_limit(&limits, res);
		if (!lim)
			continue;

		/* Is the resource outside the limits? */
		if (lim->base > res->base)
			res->base = lim->base;
		if (res->limit > lim->limit)
			res->limit = lim->limit;

		/* MEM resources need to start at the highest address manageable. */
		if (res->flags & IORESOURCE_MEM)
			res->base = resource_max(res);

		printk(BIOS_SPEW, "%s:@%s %02lx base %08llx limit %08llx\n",
			__func__, dev_path(dev), res->index, res->base, res->limit);
	}
}

device_t vga_pri = 0;
static void set_vga_bridge_bits(void)
{
	/*
	 * FIXME: Modify set_vga_bridge() so it is less PCI-centric!
	 * This function knows too much about PCI stuff, it should be just
	 * an iterator/visitor.
	 */

	/* FIXME: Handle the VGA palette snooping. */
	struct device *dev, *vga, *vga_onboard;
	struct bus *bus;

	bus = 0;
	vga = 0;
	vga_onboard = 0;

	dev = NULL;
	while ((dev = dev_find_class(PCI_CLASS_DISPLAY_VGA << 8, dev))) {
		if (!dev->enabled)
			continue;

		printk(BIOS_DEBUG, "found VGA at %s\n", dev_path(dev));

		if (dev->on_mainboard) {
			vga_onboard = dev;
		} else {
			vga = dev;
		}

		/* It isn't safe to enable all VGA cards. */
		dev->command &= ~(PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
	}

	if (!vga)
		vga = vga_onboard;

	if (CONFIG_ONBOARD_VGA_IS_PRIMARY && vga_onboard)
		vga = vga_onboard;

	/* If we prefer plugin VGA over chipset VGA, the chipset might
	   want to know. */
	if (!CONFIG_ONBOARD_VGA_IS_PRIMARY && (vga != vga_onboard) &&
		vga_onboard && vga_onboard->ops && vga_onboard->ops->disable) {
		printk(BIOS_DEBUG, "Use plugin graphics over integrated.\n");
		vga_onboard->ops->disable(vga_onboard);
	}

	if (vga) {
		/* VGA is first add-on card or the only onboard VGA. */
		printk(BIOS_DEBUG, "Setting up VGA for %s\n", dev_path(vga));
		/* All legacy VGA cards have MEM & I/O space registers. */
		vga->command |= (PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		vga_pri = vga;
		bus = vga->bus;
	}

	/* Now walk up the bridges setting the VGA enable. */
	while (bus) {
		printk(BIOS_DEBUG, "Setting PCI_BRIDGE_CTL_VGA for bridge %s\n",
		       dev_path(bus->dev));
		bus->bridge_ctrl |= PCI_BRIDGE_CTL_VGA;
		bus = (bus == bus->dev->bus) ? 0 : bus->dev->bus;
	}
}

/**
 * Assign the computed resources to the devices on the bus.
 *
 * Use the device specific set_resources() method to store the computed
 * resources to hardware. For bridge devices, the set_resources() method
 * has to recurse into every down stream buses.
 *
 * Mutual recursion:
 *	assign_resources() -> device_operation::set_resources()
 *	device_operation::set_resources() -> assign_resources()
 *
 * @param bus Pointer to the structure for this bus.
 */
void assign_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s assign_resources, bus %d link: %d\n",
	       dev_path(bus->dev), bus->secondary, bus->link_num);

	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (!curdev->enabled || !curdev->resource_list)
			continue;

		if (!curdev->ops || !curdev->ops->set_resources) {
			printk(BIOS_ERR, "%s missing set_resources\n",
			       dev_path(curdev));
			continue;
		}
		post_log_path(curdev);
		curdev->ops->set_resources(curdev);
	}
	post_log_clear();
	printk(BIOS_SPEW, "%s assign_resources, bus %d link: %d\n",
	       dev_path(bus->dev), bus->secondary, bus->link_num);
}

/**
 * Enable the resources for devices on a link.
 *
 * Enable resources of the device by calling the device specific
 * enable_resources() method.
 *
 * The parent's resources should be enabled first to avoid having enabling
 * order problem. This is done by calling the parent's enable_resources()
 * method before its children's enable_resources() methods.
 *
 * @param link The link whose devices' resources are to be enabled.
 */
static void enable_resources(struct bus *link)
{
	struct device *dev;
	struct bus *c_link;

	for (dev = link->children; dev; dev = dev->sibling) {
		if (dev->enabled && dev->ops && dev->ops->enable_resources) {
			post_log_path(dev);
			dev->ops->enable_resources(dev);
		}
	}

	for (dev = link->children; dev; dev = dev->sibling) {
		for (c_link = dev->link_list; c_link; c_link = c_link->next)
			enable_resources(c_link);
	}
	post_log_clear();
}

/**
 * Reset all of the devices on a bus and clear the bus's reset_needed flag.
 *
 * @param bus Pointer to the bus structure.
 * @return 1 if the bus was successfully reset, 0 otherwise.
 */
int reset_bus(struct bus *bus)
{
	if (bus && bus->dev && bus->dev->ops && bus->dev->ops->reset_bus) {
		bus->dev->ops->reset_bus(bus);
		bus->reset_needed = 0;
		return 1;
	}
	return 0;
}

/**
 * Scan for devices on a bus.
 *
 * If there are bridges on the bus, recursively scan the buses behind the
 * bridges. If the setting up and tuning of the bus causes a reset to be
 * required, reset the bus and scan it again.
 *
 * @param busdev Pointer to the bus device.
 */
static void scan_bus(struct device *busdev)
{
	int do_scan_bus;
	struct stopwatch sw;

	stopwatch_init(&sw);

	if (!busdev->enabled)
		return;

	printk(BIOS_SPEW, "%s scanning...\n", dev_path(busdev));

	post_log_path(busdev);

	do_scan_bus = 1;
	while (do_scan_bus) {
		struct bus *link;
		busdev->ops->scan_bus(busdev);
		do_scan_bus = 0;
		for (link = busdev->link_list; link; link = link->next) {
			if (link->reset_needed) {
				if (reset_bus(link))
					do_scan_bus = 1;
				else
					busdev->bus->reset_needed = 1;
			}
		}
	}

	printk(BIOS_DEBUG, "%s: scanning of bus %s took %ld usecs\n",
		__func__, dev_path(busdev), stopwatch_duration_usecs(&sw));
}

void scan_bridges(struct bus *bus)
{
	struct device *child;

	for (child = bus->children; child; child = child->sibling) {
		if (!child->ops || !child->ops->scan_bus)
			continue;
		scan_bus(child);
	}
}

/**
 * Determine the existence of devices and extend the device tree.
 *
 * Most of the devices in the system are listed in the mainboard devicetree.cb
 * file. The device structures for these devices are generated at compile
 * time by the config tool and are organized into the device tree. This
 * function determines if the devices created at compile time actually exist
 * in the physical system.
 *
 * For devices in the physical system but not listed in devicetree.cb,
 * the device structures have to be created at run time and attached to the
 * device tree.
 *
 * This function starts from the root device 'dev_root', scans the buses in
 * the system recursively, and modifies the device tree according to the
 * result of the probe.
 *
 * This function has no idea how to scan and probe buses and devices at all.
 * It depends on the bus/device specific scan_bus() method to do it. The
 * scan_bus() method also has to create the device structure and attach
 * it to the device tree.
 */
void dev_enumerate(void)
{
	struct device *root;

	printk(BIOS_INFO, "Enumerating buses...\n");

	root = &dev_root;

	show_all_devs(BIOS_SPEW, "Before device enumeration.");
	printk(BIOS_SPEW, "Compare with tree...\n");
	show_devs_tree(root, BIOS_SPEW, 0);

	if (root->chip_ops && root->chip_ops->enable_dev)
		root->chip_ops->enable_dev(root);

	if (!root->ops || !root->ops->scan_bus) {
		printk(BIOS_ERR, "dev_root missing scan_bus operation");
		return;
	}
	scan_bus(root);
	post_log_clear();
	printk(BIOS_INFO, "done\n");
}

/**
 * Configure devices on the devices tree.
 *
 * Starting at the root of the device tree, travel it recursively in two
 * passes. In the first pass, we compute and allocate resources (ranges)
 * required by each device. In the second pass, the resources ranges are
 * relocated to their final position and stored to the hardware.
 *
 * I/O resources grow upward. MEM resources grow downward.
 *
 * Since the assignment is hierarchical we set the values into the dev_root
 * struct.
 */
void dev_configure(void)
{
	struct resource *res;
	const struct device *root;
	const struct device *child;

	set_vga_bridge_bits();

	printk(BIOS_INFO, "Allocating resources...\n");

	root = &dev_root;

	/*
	 * Each domain should create resources which contain the entire address
	 * space for IO, MEM, and PREFMEM resources in the domain. The
	 * allocation of device resources will be done from this address space.
	 */

	/* Read the resources for the entire tree. */

	printk(BIOS_INFO, "Reading resources...\n");
	read_resources(root->link_list);
	printk(BIOS_INFO, "Done reading resources.\n");

	print_resource_tree(root, BIOS_SPEW, "After reading.");

	/* Compute resources for all domains. */
	for (child = root->link_list->children; child; child = child->sibling) {
		if (!(child->path.type == DEVICE_PATH_DOMAIN))
			continue;
		post_log_path(child);
		for (res = child->resource_list; res; res = res->next) {
			if (res->flags & IORESOURCE_FIXED)
				continue;
			if (res->flags & IORESOURCE_MEM) {
				compute_resources(child->link_list,
						  res, IORESOURCE_TYPE_MASK, IORESOURCE_MEM);
				continue;
			}
			if (res->flags & IORESOURCE_IO) {
				compute_resources(child->link_list,
						  res, IORESOURCE_TYPE_MASK, IORESOURCE_IO);
				continue;
			}
		}
	}

	/* For all domains. */
	for (child = root->link_list->children; child; child=child->sibling)
		if (child->path.type == DEVICE_PATH_DOMAIN)
			avoid_fixed_resources(child);

	/* Store the computed resource allocations into device registers ... */
	printk(BIOS_INFO, "Setting resources...\n");
	for (child = root->link_list->children; child; child = child->sibling) {
		if (!(child->path.type == DEVICE_PATH_DOMAIN))
			continue;
		post_log_path(child);
		for (res = child->resource_list; res; res = res->next) {
			if (res->flags & IORESOURCE_FIXED)
				continue;
			if (res->flags & IORESOURCE_MEM) {
				allocate_resources(child->link_list,
						   res, IORESOURCE_TYPE_MASK, IORESOURCE_MEM);
				continue;
			}
			if (res->flags & IORESOURCE_IO) {
				allocate_resources(child->link_list,
						   res, IORESOURCE_TYPE_MASK, IORESOURCE_IO);
				continue;
			}
		}
	}
	assign_resources(root->link_list);
	printk(BIOS_INFO, "Done setting resources.\n");
	print_resource_tree(root, BIOS_SPEW, "After assigning values.");

	printk(BIOS_INFO, "Done allocating resources.\n");
}

/**
 * Enable devices on the device tree.
 *
 * Starting at the root, walk the tree and enable all devices/bridges by
 * calling the device's enable_resources() method.
 */
void dev_enable(void)
{
	struct bus *link;

	printk(BIOS_INFO, "Enabling resources...\n");

	/* Now enable everything. */
	for (link = dev_root.link_list; link; link = link->next)
		enable_resources(link);

	printk(BIOS_INFO, "done.\n");
}

/**
 * Initialize a specific device.
 *
 * The parent should be initialized first to avoid having an ordering problem.
 * This is done by calling the parent's init() method before its children's
 * init() methods.
 *
 * @param dev The device to be initialized.
 */
static void init_dev(struct device *dev)
{
	if (!dev->enabled)
		return;

	if (!dev->initialized && dev->ops && dev->ops->init) {
#if IS_ENABLED(CONFIG_HAVE_MONOTONIC_TIMER)
		struct stopwatch sw;
		stopwatch_init(&sw);
#endif
		if (dev->path.type == DEVICE_PATH_I2C) {
			printk(BIOS_DEBUG, "smbus: %s[%d]->",
			       dev_path(dev->bus->dev), dev->bus->link_num);
		}

		printk(BIOS_DEBUG, "%s init ...\n", dev_path(dev));
		dev->initialized = 1;
		dev->ops->init(dev);
#if IS_ENABLED(CONFIG_HAVE_MONOTONIC_TIMER)
		printk(BIOS_DEBUG, "%s init finished in %ld usecs\n", dev_path(dev),
			stopwatch_duration_usecs(&sw));
#endif
	}
}

static void init_link(struct bus *link)
{
	struct device *dev;
	struct bus *c_link;

	for (dev = link->children; dev; dev = dev->sibling) {
		post_code(POST_BS_DEV_INIT);
		post_log_path(dev);
		init_dev(dev);
	}

	for (dev = link->children; dev; dev = dev->sibling) {
		for (c_link = dev->link_list; c_link; c_link = c_link->next)
			init_link(c_link);
	}
}

/**
 * Initialize all devices in the global device tree.
 *
 * Starting at the root device, call the device's init() method to do
 * device-specific setup, then call each child's init() method.
 */
void dev_initialize(void)
{
	struct bus *link;

	printk(BIOS_INFO, "Initializing devices...\n");

#if IS_ENABLED(CONFIG_ARCH_X86)
	/*
	 * Initialize EBDA area in ramstage if early
	 * initialization is not done.
	 */
	if (!IS_ENABLED(CONFIG_EARLY_EBDA_INIT))
		/* Ensure EBDA is prepared before Option ROMs. */
		setup_default_ebda();
#endif

	/* First call the mainboard init. */
	init_dev(&dev_root);

	/* Now initialize everything. */
	for (link = dev_root.link_list; link; link = link->next)
		init_link(link);
	post_log_clear();

	printk(BIOS_INFO, "Devices initialized\n");
	show_all_devs(BIOS_SPEW, "After init.");
}

/**
 * Finalize a specific device.
 *
 * The parent should be finalized first to avoid having an ordering problem.
 * This is done by calling the parent's final() method before its childrens'
 * final() methods.
 *
 * @param dev The device to be initialized.
 */
static void final_dev(struct device *dev)
{
	if (!dev->enabled)
		return;

	if (dev->ops && dev->ops->final) {
		printk(BIOS_DEBUG, "%s final\n", dev_path(dev));
		dev->ops->final(dev);
	}
}

static void final_link(struct bus *link)
{
	struct device *dev;
	struct bus *c_link;

	for (dev = link->children; dev; dev = dev->sibling)
		final_dev(dev);

	for (dev = link->children; dev; dev = dev->sibling) {
		for (c_link = dev->link_list; c_link; c_link = c_link->next)
			final_link(c_link);
	}
}
/**
 * Finalize all devices in the global device tree.
 *
 * Starting at the root device, call the device's final() method to do
 * device-specific cleanup, then call each child's final() method.
 */
void dev_finalize(void)
{
	struct bus *link;

	printk(BIOS_INFO, "Finalize devices...\n");

	/* First call the mainboard finalize. */
	final_dev(&dev_root);

	/* Now finalize everything. */
	for (link = dev_root.link_list; link; link = link->next)
		final_link(link);

	printk(BIOS_INFO, "Devices finalized\n");
}
