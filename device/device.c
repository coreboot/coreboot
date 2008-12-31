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
 * (Written by Yinghai Lu for Tyan)
 * Copyright (C) 2005-2006 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2007 coresystems GmbH
 * Copyright (C) 2008 Myles Watson <mylesgw@gmail.com>
 */

/*
 *      (c) 1999--2000 Martin Mares <mj@suse.cz>
 */
/* lots of mods by ron minnich (rminnich@lanl.gov), with
 * the final architecture guidance from Tom Merritt (tjm@codegen.com)
 * In particular, we changed from the one-pass original version to
 * Tom's recommended multiple-pass version. I wasn't sure about doing
 * it with multiple passes, until I actually started doing it and saw
 * the wisdom of Tom's recommendations ...
 *
 * Lots of cleanups by Eric Biederman to handle bridges, and to
 * handle resource allocation for non-pci devices.
 */

#include <console.h>
#include <io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <lib.h>
#include <spinlock.h>

/** Linked list of all devices. */
struct device *all_devices = &dev_root;

/**
 * Pointer to the last device -- computed at run time.
 * No more config tool magic.
 */
struct device **last_dev_p;

/**
 * device memory. All the device tree wil live here
 */

#define MAX_DEVICES 256
static struct device devs[MAX_DEVICES];

/**
 * the number of devices that have been allocated
 */
static int devcnt;

/**
 * The device creator.
 *
 * reserves a piece of memory for a device in the tree
 *
 * @return Pointer to the newly created device structure.
 */

static struct device *new_device(void)
{
	devcnt++;

	printk(BIOS_SPEW, "%s: devcnt %d\n", __FUNCTION__, devcnt);
	/* Should we really die here? */
	if (devcnt >= MAX_DEVICES) {
		die("Too many devices. Increase MAX_DEVICES\n");
	}

	return &devs[devcnt];
}

/**
 * The default constructor, which simply sets the ops pointer.
 *
 * Initialize device->ops of a newly allocated device structure.
 *
 * @param dev Pointer to the newly created device structure.
 * @param ops Pointer to device_operations
 */
void default_device_constructor(struct device *dev,
				const struct device_operations *ops)
{
	printk(BIOS_DEBUG, "default device constructor called\n");
	dev->ops = ops;
}

/**
 * Given a path, locate the device_operations for it from all_device_operations.
 *
 * @param id a device ID to match
 * @return Pointer to the ops or 0, if none found.
 * @see device_path
 */
struct device_operations *find_device_operations(struct device_id *id)
{
	extern struct device_operations *all_device_operations[];
	struct device_operations *c;
	int i;

	for (i = 0; all_device_operations[i]; i++) {
		printk(BIOS_SPEW, "%s: check all_device_operations[%d]\n",
		       __func__, i);
		c = all_device_operations[i];
		printk(BIOS_SPEW, "%s: cons id %s\n",
		       __func__, dev_id_string(&c->id));
		if (id_eq(&c->id, id)) {
			printk(BIOS_SPEW, "%s: match %s\n",
			       __func__, dev_id_string(&c->id));
			return c;
		}
	}

	return NULL;
}

/**
 * Initialization tasks for the device tree code.
 *
 * Sets up last_dev_p, which used to be done by magic in the config tool. Also,
 * for each of the devices, tries to find the constructor, and from there, the
 * ops, for the device.
 */
void dev_init(void)
{
	struct device *dev;
	const struct device_operations *c;

	for (dev = all_devices; dev; dev = dev->next) {
		c = dev->ops;
		if (c)
			dev->id = c->id;
		/* note the difference from the constructor function below.
		 * we are not allocating the device here, just setting the id.
		 * We set the id here because we don't want to set it in the dts
		 * as we used to. The user sees none of this work.
		 */
		if (c)
			dev->ops = c;
		last_dev_p = &dev->next;
	}
	devcnt = 0;
}

/**
 * Given a device, find a constructor function and, if found, run it.
 *
 * Given a device, use the device id in the device to find a device_operations.
 * Call the device_operations->constructor, with itself as
 * a parameter; return the result. If there is no constructor,
 * then no constructor is run.
 *
 * @param dev  Pointer to the newly created device structure.
 * @see device_path
 */
void constructor(struct device *dev)
{
	const struct device_operations *c;

	c = dev->ops;

	if (!c)
		c = find_device_operations(&dev->id);

	if (c) {
		printk(BIOS_SPEW, "%s: constructor has ID %s\n", __func__,
		       dev_id_string(&c->id));

		if (c->constructor)
			c->constructor(dev, c);
		else
			default_device_constructor(dev, c);
	} else
		printk(BIOS_INFO,
		       "No ops found and no constructor called for %s.\n",
		       dev_id_string(&dev->id));
}

spin_define(dev_lock);

/**
 * Allocate a new device structure and attach it to the device tree as a
 * child of the parent bus.
 *
 * @param parent Parent bus the newly created device is attached to.
 * @param path Path to the device to be created.
 * @param devid ID of the device we want allocated.
 * @return Pointer to the newly created device structure.
 * @see device_path
 */
struct device *alloc_dev(struct bus *parent, struct device_path *path,
			 struct device_id *devid)
{
	struct device *dev, *child;
	int link;
	spin_lock(&dev_lock);

	/* Find the last child of our parent. */
	for (child = parent->children; child && child->sibling; /* */ ) {
		child = child->sibling;
	}

	dev = new_device();
	if (!dev)
		goto out;

	memset(dev, 0, sizeof(*dev));
	dev->path = *path;
	dev->id = *devid;

	/* Initialize the back pointers in the link fields. */
	for (link = 0; link < MAX_LINKS; link++) {
		dev->link[link].dev = dev;
		dev->link[link].link = link;
	}

	/* By default devices are enabled. */
	dev->enabled = 1;

	/* Add the new device to the list of children of the bus. */
	dev->bus = parent;
	if (child) {
		child->sibling = dev;
	} else {
		parent->children = dev;
	}

	/* Append a new device to the global device list.
	 * The list is used to find devices once everything is set up.
	 */
	*last_dev_p = dev;
	last_dev_p = &dev->next;

	/* Give the device a name. */
	if (dev->id.type == DEVICE_ID_PNP &&
	    parent->dev->id.type == DEVICE_ID_PNP)
		sprintf(dev->dtsname, "%s_pnp_child_%d", parent->dev->dtsname,
			dev->path.pnp.device);
	else
		sprintf(dev->dtsname, "dynamic %s", dev_path(dev));

	/* Run the device specific constructor as last part of the chain
	 * so it gets the chance to overwrite the "inherited" values above
	 */

	constructor(dev);

      out:
	spin_unlock(&dev_lock);
	return dev;
}

/**
 * Read the resources on all devices of a given bus.
 *
 * @param bus Bus to read the resources on.
 */
void read_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s: %s(%s) read_resources bus %d link: %d\n",
	       __func__,
	       (bus->dev ? bus->dev->dtsname : "No dtsname for NULL device"),
	       (bus->dev ? dev_path(bus->dev) : "No path for NULL device"),
	       bus->secondary, bus->link);
	if (!bus->dev)
		printk(BIOS_WARNING, "%s: ERROR: bus->dev is NULL!\n",
		       __func__);

	/* Walk through all devices and find which resources they need. */
	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		int i;
		printk(BIOS_SPEW,
		       "%s: %s(%s) dtsname %s enabled %d\n",
		       __func__, bus->dev ? bus->dev->dtsname : "NOBUSDEV",
		       bus->dev ? dev_path(bus->dev) : "NOBUSDEV",
		       curdev->dtsname, curdev->enabled);
		if (!curdev->enabled) {
			continue;
		}
		if (!curdev->ops || !curdev->ops->phase4_read_resources) {
			printk(BIOS_ERR,
			       "%s: %s(%s) missing phase4_read_resources\n",
			       __func__, curdev->dtsname, dev_path(curdev));
			continue;
		}
		curdev->ops->phase4_read_resources(curdev);

		/* Read in children's resources behind the current device. */
		for (i = 0; i< curdev->links; i++)
			read_resources(&curdev->link[i]);
	}
	printk(BIOS_SPEW, "%s: %s(%s) read_resources bus %d link: %d done\n",
	       __func__, bus->dev->dtsname, dev_path(bus->dev), bus->secondary,
	       bus->link);
}

struct pick_largest_state {
	struct resource *last;
	struct device *result_dev;
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
		return;		// Skip it.
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

static struct device *largest_resource(struct bus *bus, struct resource
				       **result_res, unsigned long type_mask,
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
 * This function is the first part of the resource allocator.
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
 * - This function calculates how large the resources are behind the bridges.
 *
 * @param bus The bus we are traversing.
 * @param bridge The bridge resource which will contain the bus' resources.
 * @param type_mask This value gets anded with the resource type.
 * @param type This value must match the result of the and.
 */
void compute_resource_needs(struct bus *bus, struct resource *bridge,
			       unsigned long type_mask, unsigned long type)
{
	struct device *dev;
	struct resource *resource;
	resource_t base;
	base = align_up(bridge->base, bridge->align);

	printk(BIOS_SPEW,
	       "%s %s_%s: base: %llx size: %llx align: %d gran: %d limit: %llx\n",
	       dev_path(bus->dev), __func__,
	       (type & IORESOURCE_IO) ? "io" : (type & IORESOURCE_PREFETCH) ?
						"prefmem" : "mem",
	       base, bridge->size, bridge->align,
	       bridge->gran, bridge->limit);

	/* For each child which is a bridge, compute_resource_needs. */
	for (dev = bus->children; dev; dev = dev->sibling) {
		unsigned i;
		struct resource *child_bridge;

		if (!dev->links)
			continue;

		/* Find the resources with matching type flags. */
		for (i=0; i< dev->resources; i++){
			child_bridge = &dev->resource[i];

			if (!(child_bridge->flags & IORESOURCE_BRIDGE) ||
			     (child_bridge->flags & type_mask) != type)
				continue;

			/* Split prefetchable memory if combined.  Many domains
			 * use the same address space for prefetchable memory
			 * and non-prefetchable memory.  Bridges below them
			 * need it separated.  Add the PREFETCH flag to the
			 * type_mask and type.
			 */
			compute_resource_needs(&dev->link[0], child_bridge,
					       type_mask | IORESOURCE_PREFETCH,
					       type | (child_bridge->flags &
						       IORESOURCE_PREFETCH));
		}
	}

	/* Remember we haven't found anything yet. */
	resource = NULL;

	/* Walk through all the resources on the current bus and compute the
	 * amount of address space taken by them.  Take granularity and
	 * alignment into account.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {

		/* Size 0 resources can be skipped. */
		if (!resource->size) {
			continue;
		}

		/* Propagate the resource alignment to the bridge resource. */
		if (resource->align > bridge->align) {
			bridge->align = resource->align;
		}

		/* Propagate the resource limit to the bridge register. */
		if (bridge->limit > resource->limit) {
			bridge->limit = resource->limit;
		}

		/* I'm not sure what to do here.  I'd really like this to go
		 * away into some PCI-specific file, but I don't see how to do
		 * it.  I'm also not sure how to guarantee that larger
		 * allocations don't conflict with this address set.
		 * The example is 0x1000-0x13ff overlaps, but since the base
		 * doesn't, then this check doesn't trigger.  It wouldn't do
		 * any good, though, since you can't move it to avoid the
		 * conflict.
		 */
		if (resource->flags & IORESOURCE_IO) {
			/* Don't allow potential aliases over the legacy PCI
			 * expansion card addresses. The legacy PCI decodes
			 * only 10 bits, uses 0x100 - 0x3ff. Therefore, only
			 * 0x00 - 0xff can be used out of each 0x400 block of
			 * I/O space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/* Don't allow allocations in the VGA I/O range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}
		/* Base must be aligned. */
		base = align_up(base, resource->align);
		resource->base = base;
		base += resource->size;

		printk(BIOS_SPEW, "%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       dev_path(dev), resource->index, resource->base,
		       resource->base + resource->size - 1,
		       (resource->flags & IORESOURCE_IO) ? "io" :
		       (resource-> flags & IORESOURCE_PREFETCH) ? "prefmem" :
		        "mem");
	}
	/* Bridge resources have a minimum granularity. Round the size up to
	 * that minimum granularity so we know not to place something else at
	 * an address positively decoded by the bridge.
	 */
	bridge->size = align_up(base, bridge->gran) -
		       align_up(bridge->base, bridge->align);

	printk(BIOS_SPEW,
	       "%s %s_%s: base: %llx size: %llx align: %d gran: %d limit: %llx done\n",
	       dev_path(bus->dev), __func__,
	       (type & IORESOURCE_IO) ? "io" : (type & IORESOURCE_PREFETCH) ?
						"prefmem" : "mem",
	       base, bridge->size, bridge->align,
	       bridge->gran, bridge->limit);
}

/**
 * This function is the second part of the resource allocator.
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
 * - This function assigns the resources a value.
 *
 * @param bus The bus we are traversing.
 * @param bridge The bridge resource which must contain the bus' resources.
 * @param type_mask This value gets anded with the resource type.
 * @param type This value must match the result of the and.
 */
void assign_resource_values(struct bus *bus, struct resource *bridge,
			       unsigned long type_mask, unsigned long type)
{
	struct device *dev;
	struct resource *resource;
	resource_t base;
	base = bridge->base;

	printk(BIOS_SPEW,
	       "%s %s_%s: base:%llx size:%llx align:%d gran:%d limit:%llx\n",
	       dev_path(bus->dev), __func__,
	       (type & IORESOURCE_IO) ? "io" : (type & IORESOURCE_PREFETCH) ?
						"prefmem" : "mem",
	       base, bridge->size, bridge->align,
	       bridge->gran, bridge->limit);

	/* Remember we haven't found anything yet. */
	resource = NULL;

	/* Walk through all the resources on the current bus and allocate them
	 * address space.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {

		/* Size 0 resources can be skipped. */
		if (!resource->size) {
			continue;
		}

		if (resource->flags & IORESOURCE_IO) {
			/* Don't allow potential aliases over the legacy PCI
			 * expansion card addresses. The legacy PCI decodes
			 * only 10 bits, uses 0x100 - 0x3ff. Therefore, only
			 * 0x00 - 0xff can be used out of each 0x400 block of
			 * I/O space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/* Don't allow allocations in the VGA I/O range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}


		if ((align_up(base, resource->align) + resource->size - 1) <=
		     resource->limit) {
			/* Base must be aligned. */
			base = align_up(base, resource->align);
			resource->base = base;
			resource->flags |= IORESOURCE_ASSIGNED;
			resource->flags &= ~IORESOURCE_STORED;
			base += resource->size;
		} else {
			printk(BIOS_ERR, "!! Resource didn't fit !!\n");
			printk(BIOS_ERR, "   aligned base %llx size %llx limit %llx\n",
			       align_up(base, resource->align), resource->size, resource->limit);
			printk(BIOS_ERR, "   %llx needs to be <= %llx (limit)\n",
			       (align_up(base, resource->align)+resource->size)-1, resource->limit);
			printk(BIOS_ERR, "   %s%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       (resource->flags & IORESOURCE_ASSIGNED) ? "Assigned: "
							       : "",
		       dev_path(dev), resource->index, resource->base,
		       resource->base + resource->size - 1,
		       (resource->flags & IORESOURCE_IO) ? "io" :
		       (resource-> flags & IORESOURCE_PREFETCH) ? "prefmem" :
		        "mem");
		}

		printk(BIOS_SPEW, "%s%s %02lx *  [0x%llx - 0x%llx] %s\n",
		       (resource->flags & IORESOURCE_ASSIGNED) ? "Assigned: "
							       : "",
		       dev_path(dev), resource->index, resource->base,

			resource->size?  resource->base + resource->size - 1 :
			resource->base,

		       (resource->flags & IORESOURCE_IO) ? "io" :
		       (resource-> flags & IORESOURCE_PREFETCH) ? "prefmem" :
		        "mem");
	}
	/* A PCI bridge resource does not need to be a power of two size, but
	 * it does have a minimum granularity. Round the size up to that
	 * minimum granularity so we know not to place something else at an
	 * address positively decoded by the bridge.
	 */

	bridge->flags |= IORESOURCE_ASSIGNED;

	printk(BIOS_SPEW,
	       "%s %s_%s: next_base: %llx size: %llx align: %d gran: %d done\n",
	       dev_path(bus->dev), __func__,
	       (type & IORESOURCE_IO) ? "io" : (type & IORESOURCE_PREFETCH) ?
						"prefmem" : "mem",
	       base, bridge->size, bridge->align,
	       bridge->gran);

	/* For each child which is a bridge, assign_resource_values. */
	for (dev = bus->children; dev; dev = dev->sibling) {
		unsigned i;
		struct resource *child_bridge;

		if (!dev->links)
			continue;

		/* Find the resources with matching type flags. */
		for (i=0; i< dev->resources; i++){
			child_bridge = &dev->resource[i];

			if (!(child_bridge->flags & IORESOURCE_BRIDGE) ||
			     (child_bridge->flags & type_mask) != type)
				continue;

			/* Split prefetchable memory if combined.  Many domains
			 * use the same address space for prefetchable memory
			 * and non-prefetchable memory.  Bridges below them
			 * need it separated.  Add the PREFETCH flag to the
			 * type_mask and type.
			 */
			assign_resource_values(&dev->link[0], child_bridge,
					       type_mask | IORESOURCE_PREFETCH,
					       type | (child_bridge->flags &
						       IORESOURCE_PREFETCH));
		}
	}
}

struct constraints {
	struct resource pref, io, mem;
};

static void constrain_resources(struct device *dev, struct constraints* limits)
{
	struct device *child;
	struct resource *res;
	struct resource *lim;
	int i;

#ifdef CONFIG_PCI_64BIT_PREF_MEM
	#define MEM_MASK (IORESOURCE_PREFETCH | IORESOURCE_MEM)
#else
	#define MEM_MASK (IORESOURCE_MEM)
#endif
#define IO_MASK (IORESOURCE_IO)
#define PREF_TYPE (IORESOURCE_PREFETCH | IORESOURCE_MEM)
#define MEM_TYPE (IORESOURCE_MEM)
#define IO_TYPE (IORESOURCE_IO)

	/* Descend into every child and look for fixed resources. */
	for (child=dev->link[0].children; child; child = child->sibling) {
		constrain_resources(child, limits);
		for (i = 0; i<child->resources; i++) {
			res = &child->resource[i];
			if (!(res->flags & IORESOURCE_FIXED))
				continue;

			/* PREFETCH, MEM, or I/O - skip any others. */
			if ((res->flags & MEM_MASK) == PREF_TYPE)
				lim = &limits->pref;
			else if ((res->flags & MEM_MASK) == MEM_TYPE)
				lim = &limits->mem;
			else if ((res->flags & IO_MASK) == IO_TYPE)
				lim = &limits->io;
			else
				continue;

			/* Is it already outside the limits? */
			if (res->size &&
			    (((res->base + res->size -1) < lim->base) ||
			     (res->base > lim->limit)))
				continue;

			/* Choose to be above or below fixed resources.  This
			 * check is signed so that "negative" amounts of space
			 * are handled correctly.
			 */
			if ((s64)(lim->limit - (res->base + res->size -1)) >
			    (s64)(res->base - lim->base))
				lim->base = res->base + res->size;
			else
				lim->limit = res->base -1;
		}
	}
}

static void avoid_fixed_resources(struct device *dev)
{
	struct constraints limits;
	struct resource *res;
	int i;

	/* Initialize constraints to maximum size. */

	limits.pref.base = 0;
	limits.pref.limit = 0xffffffffffffffffULL;
	limits.io.base = 0;
	limits.io.limit = 0xffffffffffffffffULL;
	limits.mem.base = 0;
	limits.mem.limit = 0xffffffffffffffffULL;

	/* Constrain the limits to dev's initial resources. */
	for (i = 0; i<dev->resources; i++) {
		res = &dev->resource[i];
		if ((res->flags & IORESOURCE_FIXED) ||
		    !(res->flags & IORESOURCE_BRIDGE))
			continue;
		if ((res->flags & MEM_MASK) == PREF_TYPE &&
		    (res->limit < limits.pref.limit))
			limits.pref.limit = res->limit;
		if ((res->flags & MEM_MASK) == MEM_TYPE &&
		    (res->limit < limits.mem.limit))
			limits.mem.limit = res->limit;
		if ((res->flags & IO_MASK) == IO_TYPE &&
		    (res->limit < limits.io.limit))
			limits.io.limit = res->limit;
	}

	/* Look through the tree for fixed resources and update the limits. */
	constrain_resources(dev, &limits);

	/* Update dev's resources with new limits. */
	for (i = 0; i<dev->resources; i++) {
		struct resource *lim;
		res = &dev->resource[i];

		if ((res->flags & IORESOURCE_FIXED) ||
		    !(res->flags & IORESOURCE_BRIDGE))
			continue;

		/* PREFETCH, MEM, or I/O - skip any others. */
		if ((res->flags & MEM_MASK) == PREF_TYPE)
			lim = &limits.pref;
		else if ((res->flags & MEM_MASK) == MEM_TYPE)
			lim = &limits.mem;
		else if ((res->flags & IO_MASK) == IO_TYPE)
			lim = &limits.io;
		else
			continue;

		/* Is the resource outside the limits? */
		if ( lim->base > res->base )
			res->base = lim->base;
		if ( res->limit > lim->limit )
			res->limit = lim->limit;
	}
}

#ifdef CONFIG_PCI_OPTION_ROM_RUN
struct device *vga_pri = 0;
int vga_inited = 0;
static void allocate_vga_resource(void)
{
#warning Modify allocate_vga_resource so it is less PCI centric.
	// FIXME: This function knows too much about PCI stuff,
	// it should just be an iterator/visitor.

	/* FIXME: Handle the VGA palette snooping. */
	struct device *dev, *vga, *vga_onboard, *vga_first, *vga_last;
	struct bus *bus;
	bus = 0;
	vga = 0;
	vga_onboard = 0;
	vga_first = 0;
	vga_last = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if (!dev->enabled)
			continue;
		if (((dev->class >> 16) == PCI_BASE_CLASS_DISPLAY) &&
		    ((dev->class >> 8) != PCI_CLASS_DISPLAY_OTHER)) {
			if (!vga_first) {
				if (dev->on_mainboard) {
					vga_onboard = dev;
				} else {
					vga_first = dev;
				}
			} else {
				if (dev->on_mainboard) {
					vga_onboard = dev;
				} else {
					vga_last = dev;
				}
			}

			/* It isn't safe to enable other VGA cards. */
			dev->command &= ~(PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		}
	}

	vga = vga_last;

	if (!vga) {
		vga = vga_first;
	}
#ifdef CONFIG_INITIALIZE_ONBOARD_VGA_FIRST
	if (vga_onboard)	// Will use on board VGA as pri.
#else
	if (!vga)		// Will use last add on adapter as pri.
#endif
	{
		vga = vga_onboard;
	}

	if (vga) {
		/* VGA is first add on card or the only onboard VGA. */
		printk(BIOS_DEBUG, "Allocating VGA resource %s\n",
		       dev_path(vga));
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

#endif

/**
 * Assign the computed resources to the devices on the bus.
 *
 * Use the device specific set_resources method to store the computed
 * resources to hardware. For bridge devices, the set_resources() method
 * has to recurse into every down stream buses.
 *
 * Mutual recursion:
 *	phase4_set_resources() -> device_operation::set_resources()
 *	device_operation::set_resources() -> phase4_set_resources()
 *
 * @param bus Pointer to the structure for this bus.
 */
void phase4_set_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s(%s) %s, bus %d link: %d\n", __func__,
	       bus->dev->dtsname, dev_path(bus->dev), bus->secondary,
	       bus->link);

	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (!curdev->enabled || !curdev->resources) {
			continue;
		}
		if (!curdev->ops) {
			printk(BIOS_WARNING, "%s(%s) missing ops\n",
			       curdev->dtsname, dev_path(curdev));
			continue;
		}
		if (!curdev->ops->phase4_set_resources) {
			printk(BIOS_WARNING,
			       "%s(%s) ops has no phase4_set_resources\n",
			       curdev->dtsname, dev_path(curdev));
			continue;
		}
		curdev->ops->phase4_set_resources(curdev);
	}
	printk(BIOS_SPEW, "%s(%s) %s done, bus %d link: %d\n", __func__,
	       bus->dev->dtsname, dev_path(bus->dev), bus->secondary,
	       bus->link);
}

/**
 * Enable the resources of the device by calling the device specific
 * phase5() method.
 *
 * The parent's resources should be enabled first to avoid having enabling
 * order problem. This is done by calling the parent's phase5() method and
 * let that method to call it's children's phase5() method via the (global)
 * phase5_children().
 *
 * Indirect mutual recursion:
 *	dev_phase5() -> device_operations::phase5()
 *	device_operations::phase5() -> phase5_children()
 *	phase5_children() -> dev_phase5()
 *
 * @param dev The device whose resources are to be enabled.
 */
void dev_phase5(struct device *dev)
{
	if (!dev->enabled) {
		return;
	}
	if (!dev->ops) {
		printk(BIOS_WARNING, "%s: %s(%s) missing ops\n",
		       __FUNCTION__, dev->dtsname, dev_path(dev));
		return;
	}
	if (!dev->ops->phase5_enable_resources) {
		printk(BIOS_WARNING,
		       "%s: %s(%s) ops are missing phase5_enable_resources\n",
		       __FUNCTION__, dev->dtsname, dev_path(dev));
		return;
	}

	dev->ops->phase5_enable_resources(dev);
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
 * Do very early setup for all devices in the global device list.
 *
 * Starting at the first device on the global device link list,
 * walk the list and call the device's phase1() method to do very
 * early setup.
 */
void dev_phase1(void)
{
	struct device *dev;

	post_code(POST_STAGE2_PHASE1_ENTER);
	printk(BIOS_DEBUG, "Phase 1: Very early setup...\n");
	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->phase1_set_device_operations) {
			dev->ops->phase1_set_device_operations(dev);
		}
	}
	post_code(POST_STAGE2_PHASE1_DONE);
	printk(BIOS_DEBUG, "Phase 1: done\n");
	post_code(POST_STAGE2_PHASE1_EXIT);
}

/**
 * Do early setup for all devices in the global device list.
 *
 * Starting at the first device on the global device link list,
 * walk the list and call the device's phase2() method to do
 * early setup.
 */
void dev_phase2(void)
{
	struct device *dev;

	post_code(POST_STAGE2_PHASE2_ENTER);
	printk(BIOS_DEBUG, "Phase 2: Early setup...\n");
	for (dev = all_devices; dev; dev = dev->next) {
		printk(BIOS_SPEW,
		       "%s: dev %s: ops %sNULL ops->phase2_fixup %s\n",
		       __FUNCTION__, dev->dtsname, dev->ops ? "NOT " : "",
		       dev->ops ? (dev->ops->phase2_fixup ? "NOT NULL" : "NULL")
		       : "N/A");
		if (dev->ops && dev->ops->phase2_fixup) {
			printk(BIOS_SPEW, "Calling phase2 phase2_fixup...\n");
			dev->ops->phase2_fixup(dev);
			printk(BIOS_SPEW, "phase2_fixup done\n");
		}
	}

	post_code(POST_STAGE2_PHASE2_DONE);
	printk(BIOS_DEBUG, "Phase 2: Done.\n");
	post_code(POST_STAGE2_PHASE2_EXIT);
}

/**
 * Scan for devices on a bus.
 *
 * If there are bridges on the bus, recursively scan the buses behind the
 * bridges. If the setting up and tuning of the bus causes a reset to be
 * required, reset the bus and scan it again.
 *
 * @param busdevice Pointer to the bus device.
 * @param max Current bus number.
 * @return The maximum bus number found, after scanning all subordinate buses.
 */
unsigned int dev_phase3_scan(struct device *busdevice, unsigned int max)
{
	unsigned int new_max;
	int do_phase3;
	post_code(POST_STAGE2_PHASE3_SCAN_ENTER);
	if (!busdevice || !busdevice->enabled ||
	    !busdevice->ops || !busdevice->ops->phase3_scan) {
		printk(BIOS_INFO, "%s: busdevice %s: enabled %d ops %s\n",
		       __FUNCTION__, busdevice ? busdevice->dtsname : "NULL",
		       busdevice ? busdevice->enabled : 0,
		       busdevice ? (busdevice->ops?
				    "NOT NULL" : "NULL") : "N/A");
		printk(BIOS_INFO, "%s: can not scan from here, returning %d\n",
		       __FUNCTION__, max);
		return max;
	}

	do_phase3 = 1;
	while (do_phase3) {
		int link;
		printk(BIOS_INFO, "%s: scanning %s(%s)\n", __FUNCTION__,
		       busdevice->dtsname, dev_path(busdevice));
#warning do we call phase3_enable here.
		new_max = busdevice->ops->phase3_scan(busdevice, max);
		do_phase3 = 0;
		/* do we *ever* use this path */
		for (link = 0; link < busdevice->links; link++) {
			if (busdevice->link[link].reset_needed) {
				if (reset_bus(&busdevice->link[link])) {
					do_phase3 = 1;
				} else {
					busdevice->bus->reset_needed = 1;
				}
			}
		}
	}
	post_code(POST_STAGE2_PHASE3_SCAN_EXIT);
	printk(BIOS_INFO, "%s: returning %d\n", __FUNCTION__, max);
	return new_max;
}

/**
 * Determine the existence of devices and extend the device tree.
 *
 * Most of the devices in the system are listed in the mainboard dts
 * file. The device structures for these devices are generated at compile
 * time by the config tool and are organized into the device tree, statictree.c.
 * This function determines if the devices created at compile time actually
 * exist in the physical system.
 *
 * For devices in the physical system but not listed in the dts, the device
 * structures have to be created at run time and attached to the device tree.
 *
 * This function starts from the root device 'dev_root', scan the buses in
 * the system recursively, modify the device tree according to the result of
 * the probe.
 *
 * This function has no idea how to scan and probe buses and devices at all.
 * It depends on the bus/device specific scan_bus() method to do it. The
 * scan_bus() method also has to create the device structure and attach
 * it to the device tree.
 */
void dev_root_phase3(void)
{
	struct device *root;
	unsigned int subordinate;

	printk(BIOS_INFO, "Phase 3: Enumerating buses...\n");
	root = &dev_root;

	if (root->ops && root->ops->phase3_chip_setup_dev) {
		root->ops->phase3_chip_setup_dev(root);
	}
	post_code(POST_STAGE2_PHASE3_MIDDLE);
	if (!root->ops) {
		printk(BIOS_ERR,
		       "dev_root_phase3 missing 'ops' initialization\nPhase 3: Failed.\n");
		return;
	}
	if (!root->ops->phase3_scan) {
		printk(BIOS_ERR,
		       "dev_root ops struct missing 'phase3' initialization in ops structure\nPhase 3: Failed.");
		return;
	}
	subordinate = dev_phase3_scan(root, 0);
	printk(BIOS_INFO, "Phase 3: Done.\n");
}

void resource_tree(const struct device *const root, int debug_level, int depth)
{
	int i = 0, link = 0;
	const struct device const *child;
	char indent[30];	/* If your tree has more levels, it's wrong. */

	for (i = 0; i < depth + 1 && i < 29; i++)
		indent[i] = ' ';
	indent[i] = '\0';

	printk(BIOS_DEBUG, "%s%s links %x child on link 0 %s\n",
	       indent, dev_path(root), root->links,
	       root->link[0].children ? root->link[0].children->
	       dtsname : "NULL");
	for (i = 0; i < root->resources; i++) {
		printk(BIOS_DEBUG,
		       "%s%s resource base %llx size %llx align %d gran %d limit %llx flags %lx index %lx\n",
		       indent, dev_path(root), root->resource[i].base,
		       root->resource[i].size, root->resource[i].align,
		       root->resource[i].gran, root->resource[i].limit,
		       root->resource[i].flags, root->resource[i].index);
	}

	for (link = 0; link < root->links; link++) {
		for (child = root->link[link].children; child;
		     child = child->sibling)
			resource_tree(child, debug_level, depth + 1);
	}
}

void print_resource_tree(const struct device *const root, int debug_level,
			 const char *msg)
{
	/* Bail if root is null. */
	if (!root) {
		printk(debug_level, "%s passed NULL for root!\n", __func__);
		return;
	}

	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show resources in subtree (%s)...%s\n",
		    root->dtsname, msg))
		return;
	resource_tree(root, debug_level, 0);
}

/**
 * Allocate resources.
 *
 * Starting at the root of the device tree, travel it recursively in four
 * passes. In the first pass, we read all the resources.  In the second pass we
 * compute the resource needs.  In the third pass we assign final values to the
 * resources.  In the fourth pass we set them.
 *
 * I/O resources start at the bottom of the domain's resource and grow upward.
 * MEM resources start at the top of the domain's resource and grow downward.
 *
 */
void dev_phase4(void)
{
	struct resource *res;
	struct device *root;
	struct device * child;
	int i;

	printk(BIOS_INFO, "Phase 4: Allocating resources...\n");

	root = &dev_root;

	/* Each domain should create resources which contain the entire address
	 * space for IO, MEM, and PREFMEM resources in the domain. The
	 * allocation of device resources will be done from this address space.
	 */

	printk(BIOS_INFO, "Phase 4: Reading resources...\n");

	/* Read the resources for the entire tree. */
	read_resources(&root->link[0]);

	printk(BIOS_INFO, "Phase 4: Done reading resources.\n");

	printk(BIOS_INFO, "Phase 4: Constrain resources.\n");

	/* For all domains. */
	for (child = root->link[0].children; child;
	     child=child->sibling)
		if (child->path.type == DEVICE_PATH_PCI_DOMAIN)
			avoid_fixed_resources(child);

	print_resource_tree(root, BIOS_DEBUG, "Original.");

	/* Compute resources for all domains. */
	for (child = root->link[0].children; child; child=child->sibling) {
		if (!(child->path.type == DEVICE_PATH_PCI_DOMAIN))
			continue;
		for (i=0; i< child->resources; i++) {
			res = &child->resource[i];
			if ( res->flags & IORESOURCE_FIXED )
				continue;
			if ( res->flags & IORESOURCE_PREFETCH ) {
				compute_resource_needs(&child->link[0],
					       res, MEM_MASK, PREF_TYPE);
				continue;
			}
			if ( res->flags & IORESOURCE_MEM ) {
				compute_resource_needs(&child->link[0],
					       res, MEM_MASK, MEM_TYPE);
				continue;
			}
			if ( res->flags & IORESOURCE_IO ) {
				compute_resource_needs(&child->link[0],
					       res, IO_MASK, IO_TYPE);
				continue;
			}
		}
	}

	print_resource_tree(root, BIOS_DEBUG, "After summations.");

	/* Now we need to adjust the resources. The issue is that mem grows
	 * downward. Reallocate the MEM resources with the highest addresses
	 * I can manage.
	 */
	for (child = root->link[0].children; child; child=child->sibling) {
		if (child->path.type != DEVICE_PATH_PCI_DOMAIN)
			continue;
		for (i=0; i< child->resources; i++) {
			res = &child->resource[i];
			if (!(res->flags & IORESOURCE_MEM) ||
			    res->flags & IORESOURCE_FIXED )
				continue;
			res->base = resource_max(res);
		}
	}

#ifdef CONFIG_PCI_OPTION_ROM_RUN
	/* Allocate the VGA I/O resource. */
	allocate_vga_resource();
	print_resource_tree(root, BIOS_DEBUG, "After VGA.");
#endif

	/* Assign values to the resources for all domains. */
	/* If the domain has a prefetchable memory resource, use it. */
	for (child = root->link[0].children; child; child=child->sibling) {
		if (!(child->path.type == DEVICE_PATH_PCI_DOMAIN))
			continue;
		for (i=0; i< child->resources; i++) {
			res = &child->resource[i];
			if ( res->flags & IORESOURCE_FIXED )
				continue;
			if ( res->flags & IORESOURCE_PREFETCH ) {
				assign_resource_values(&child->link[0],
					       res, MEM_MASK, PREF_TYPE);
				continue;
			}
			if ( res->flags & IORESOURCE_MEM ) {
				assign_resource_values(&child->link[0],
					       res, MEM_MASK, MEM_TYPE);
				continue;
			}
			if ( res->flags & IORESOURCE_IO ) {
				assign_resource_values(&child->link[0],
					       res, IO_MASK, IO_TYPE);
				continue;
			}
		}
	}

	print_resource_tree(root, BIOS_DEBUG, "After assigning values.");

	/* Store the computed resource allocations into device registers. */
	printk(BIOS_INFO, "Phase 4: Setting resources...\n");
	phase4_set_resources(&root->link[0]);
	print_resource_tree(root, BIOS_DEBUG, "After setting resources.");

	printk(BIOS_INFO, "Phase 4: Done allocating resources.\n");
}

/**
 * Enable devices on the device tree.
 *
 * Starting at the root, walk the tree and enable all devices/bridges by
 * calling the device's enable_resources() method.
 */
void dev_root_phase5(void)
{
	printk(BIOS_INFO, "Phase 5: Enabling resources...\n");

	/* Now enable everything. */
	dev_phase5(&dev_root);

	printk(BIOS_INFO, "Phase 5: Done.\n");
}

/**
 * Initialize all devices in the global device list.
 *
 * Starting at the first device on the global device link list, walk the list
 * and call the device's init() method to do device specific setup.
 */
void dev_phase6(void)
{
	struct device *dev;

	printk(BIOS_INFO, "Phase 6: Initializing devices...\n");
	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->enabled && dev->ops && dev->ops->phase6_init) {
			if (dev->path.type == DEVICE_PATH_I2C) {
				printk(BIOS_DEBUG, "Phase 6: smbus: %s[%d]->",
				       dev_path(dev->bus->dev), dev->bus->link);
			}
			printk(BIOS_DEBUG, "Phase 6: %s init.\n",
			       dev_path(dev));
			dev->ops->phase6_init(dev);
		}
	}
	printk(BIOS_INFO, "Phase 6: Devices initialized.\n");
}

void show_devs_tree(struct device *dev, int debug_level, int depth, int linknum)
{
	char depth_str[20] = "";
	int i;
	struct device *sibling;
	for (i = 0; i < depth; i++)
		depth_str[i] = ' ';
	depth_str[i] = '\0';
	printk(debug_level, "%s%s(%s): enabled %d, %d resources\n",
	       depth_str, dev->dtsname, dev_path(dev), dev->enabled,
	       dev->resources);
	for (i = 0; i < dev->links; i++) {
		for (sibling = dev->link[i].children; sibling;
		     sibling = sibling->sibling)
			show_devs_tree(sibling, debug_level, depth + 1, i);
	}
}

void show_all_devs_tree(int debug_level, const char *msg)
{
	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show all devs in tree form...%s\n", msg))
		return;
	show_devs_tree(all_devices, debug_level, 0, -1);
}

void show_devs_subtree(struct device *root, int debug_level, const char *msg)
{
	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show all devs in subtree %s...%s\n",
		    root->dtsname, msg))
		return;
	printk(debug_level, "%s\n", msg);
	show_devs_tree(root, debug_level, 0, -1);
}

void show_all_devs(int debug_level, const char *msg)
{
	struct device *dev;

	/* Bail if not printing to screen. */
	if (!printk(debug_level, "Show all devs...%s\n", msg))
		return;
	for (dev = all_devices; dev; dev = dev->next) {
		printk(debug_level,
		       "%s(%s): enabled %d, %d resources\n",
		       dev->dtsname, dev_path(dev), dev->enabled,
		       dev->resources);
	}
}

void show_one_resource(int debug_level, struct device *dev,
		       struct resource *resource, const char *comment)
{
	char buf[10];
	unsigned long long base, end;
	base = resource->base;
	end = resource_end(resource);
	buf[0] = '\0';
	if (resource->flags & IORESOURCE_BRIDGE) {
#if PCI_BUS_SEGN_BITS
		sprintf(buf, "bus %04x:%02x ", dev->bus->secondary >> 8,
			dev->link[0].secondary & 0xff);
#else
		sprintf(buf, "bus %02x ", dev->link[0].secondary);
#endif
	}
	printk(debug_level, "%s %02lx <- [0x%010llx - 0x%010llx] "
	       "size 0x%08Lx gran 0x%02x %s%s%s\n",
	       dev_path(dev), resource->index, base, end,
	       resource->size, resource->gran, buf,
	       resource_type(resource), comment);

}

void show_all_devs_resources(int debug_level, const char* msg)
{
	struct device *dev;

	if(!printk(debug_level, "Show all devs with resources...%s\n", msg))
		return;

	for (dev = all_devices; dev; dev = dev->next) {
		int i;
		printk(debug_level,
		       "%s(%s): enabled %d, %d resources\n",
		       dev->dtsname, dev_path(dev), dev->enabled,
		       dev->resources);
		for (i = 0; i < dev->resources; i++)
			show_one_resource(debug_level, dev, &dev->resource[i], "");
	}
}
