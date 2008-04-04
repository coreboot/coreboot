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
 * The upper limit of MEM resource of the devices.
 * Reserve 20M for the system.
 */
#define DEVICE_MEM_HIGH 0xFEBFFFFFUL

/**
 * The lower limit of I/O resource of the devices.
 * Reserve 4K for ISA/Legacy devices.
 */
#define DEVICE_IO_START 0x1000

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
	if (devcnt>=MAX_DEVICES) {
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
void default_device_constructor(struct device *dev, struct device_operations *ops)
{
	printk(BIOS_DEBUG, "default device constructor called\n");
	dev->ops = ops;
}

/**
 * Given a path, locate the device_operations  for it from all_device_operations..
 *
 * @param path Path to the device to be created.
 * @return Pointer to the ops or 0, if none found.
 * @see device_path
 */
struct device_operations *find_device_operations(struct device_id *id)
{
	extern struct device_operations *all_device_operations[];
	struct device_operations *c;
	int i;

	for (i = 0; all_device_operations[i]; i++) {
		printk(BIOS_SPEW, "%s: check all_device_operations[i] %p\n",
		       __func__, all_device_operations[i]);
		c = all_device_operations[i];
		printk(BIOS_SPEW, "%s: cons %p, cons id %s\n",
		       __func__, c, dev_id_string(&c->id));
		if (id_eq(&c->id, id)) {
			printk(BIOS_SPEW, "%s: match\n", __func__);
			return c;
		}
	}

	return NULL;
}

/**
 * Initialization tasks for the device tree code. 
 * 
 * Sets up last_dev_p, which used to be done by
 * Fucking Magic (FM) in the config tool. Also, for each of the 
 * devices, tries to find the constructor, and from there, the ops, 
 * for the device. 
 */
void dev_init(void)
{
	struct device *dev;
	struct device_operations *c;

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
 * @param path Path to the device to be created.
 * @see device_path
 */
void constructor(struct device *dev)
{
	struct device_operations *c;

	c = dev->ops;

	if (!c)
		c = find_device_operations(&dev->id);

	printk(BIOS_SPEW, "%s: constructor is %p\n", __func__, c);
 
	if(c) {
		if(c->constructor)
			c->constructor(dev, c);
		else
			default_device_constructor(dev, c);
	}
	else
		printk(BIOS_INFO, "No ops found and no constructor called for %s.\n", 
			dev_id_string(&dev->id));
}

/**
 * Allocate a new device structure and attach it to the device tree as a
 * child of the parent bus.
 *
 * @param parent Parent bus the newly created device is attached to.
 * @param path Path to the device to be created.
 * @param devid TODO
 * @return Pointer to the newly created device structure.
 * @see device_path
 */

spin_define(dev_lock);

struct device *alloc_dev(struct bus *parent, struct device_path *path,
			 struct device_id *devid)
{
	struct device *dev, *child;
	int link;
	spin_lock(&dev_lock);

	/* Find the last child of our parent. */
	for (child = parent->children; child && child->sibling; /* */) {
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
static void read_resources(struct bus *bus)
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
		unsigned int links;
		int i;
		printk(BIOS_SPEW,
		       "%s: %s(%s) dtsname %s have_resources %d enabled %d\n",
		       __func__, bus->dev->dtsname, dev_path(bus->dev),
		       curdev->dtsname,
		       curdev->have_resources, curdev->enabled);
		if (curdev->have_resources) {
			continue;
		}
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
		curdev->have_resources = 1;

		/* Read in subtractive resources behind the current device. */
		links = 0;
		for (i = 0; i < curdev->resources; i++) {
			struct resource *resource;
			unsigned int link;
			resource = &curdev->resource[i];
			if (!(resource->flags & IORESOURCE_SUBTRACTIVE))
				continue;
			link = IOINDEX_SUBTRACTIVE_LINK(resource->index);
			if (link > MAX_LINKS) {
				printk(BIOS_ERR,
				       "%s subtractive index on link: %d\n",
				       dev_path(curdev), link);
				continue;
			}
			if (!(links & (1 << link))) {
				links |= (1 << link);
				read_resources(&curdev->link[link]);
			}
		}
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
	state.result_dev = 0;
	state.result = 0;
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
 * @param bus TODO
 * @param bridge TODO
 * @param type_mask TODO
 * @param type TODO
 */
void compute_allocate_resource(struct bus *bus, struct resource *bridge,
			       unsigned long type_mask, unsigned long type)
{
	struct device *dev;
	struct resource *resource;
	resource_t base;
	unsigned long align, min_align;
	min_align = 0;
	base = bridge->base;

	printk(BIOS_SPEW,
	       "%s compute_allocate_%s: base: %08llx size: %08llx align: %d gran: %d\n",
	       dev_path(bus->dev),
	       (bridge->flags & IORESOURCE_IO) ? "io" : (bridge->flags & IORESOURCE_PREFETCH) ? "prefmem" : "mem",
	       base, bridge->size, bridge->align, bridge->gran);

	/* We want different minimum alignments for different kinds of
	 * resources. These minimums are not device type specific but
	 * resource type specific.
	 */
	if (bridge->flags & IORESOURCE_IO) {
		min_align = log2(DEVICE_IO_ALIGN);
	}
	if (bridge->flags & IORESOURCE_MEM) {
		min_align = log2(DEVICE_MEM_ALIGN);
	}

	/* Make certain we have read in all of the resources. */
	read_resources(bus);

	/* Remember we haven't found anything yet. */
	resource = 0;

	/* Walk through all the devices on the current bus and 
	 * compute the addresses.
	 */
	while ((dev = largest_resource(bus, &resource, type_mask, type))) {
		resource_t size;

		/* Do NOT, I repeat do not, ignore resources which have zero
		 * size. If they need to be ignored dev->read_resources should
		 * not even return them. Some resources must be set even when
		 * they have no size. PCI bridge resources are a good example
		 * of this.
		 */
		/* Propagate the resource alignment to the bridge register. */
		if (resource->align > bridge->align) {
			bridge->align = resource->align;
		}

		/* Make certain we are dealing with a good minimum size. */
		size = resource->size;
		align = resource->align;
		if (align < min_align) {
			align = min_align;
		}

		if (resource->flags & IORESOURCE_FIXED) {
			continue;
		}

		/* Propagate the resource limit to the bridge register. */
		if (bridge->limit > resource->limit) {
			bridge->limit = resource->limit;
		}

		/* Artificially deny limits between DEVICE_MEM_HIGH and 0xffffffff. */
		if ((bridge->limit > DEVICE_MEM_HIGH)
		    && (bridge->limit <= 0xffffffff)) {
			bridge->limit = DEVICE_MEM_HIGH;
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
		if (((align_up(base, align) + size) - 1) <= resource->limit) {
			/* Base must be aligned to size. */
			base = align_up(base, align);
			resource->base = base;
			resource->flags |= IORESOURCE_ASSIGNED;
			resource->flags &= ~IORESOURCE_STORED;
			base += size;

			printk(BIOS_SPEW,
			       "%s %02lx *  [0x%08llx - 0x%08llx] %s\n",
			       dev_path(dev),
			       resource->index,
			       resource->base,
			       resource->base + resource->size - 1,
			       (resource->flags & IORESOURCE_IO) ? "io" :
			       (resource->
				flags & IORESOURCE_PREFETCH) ? "prefmem" :
			       "mem");
		}
	}
	/* A PCI bridge resource does not need to be a power of two size, but
	 * it does have a minimum granularity. Round the size up to that
	 * minimum granularity so we know not to place something else at an
	 * address positively decoded by the bridge.
	 */
	bridge->size = align_up(base, bridge->gran) - bridge->base;

	printk(BIOS_SPEW,
	       "%s compute_allocate_%s: base: %08llx size: %08llx align: %d gran: %d done\n",
	       dev_path(bus->dev),
	       (bridge->flags & IORESOURCE_IO) ? "io" : (bridge->flags & IORESOURCE_PREFETCH) ? "prefmem" : "mem",
	       base, bridge->size, bridge->align, bridge->gran);
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
 *	assign_resources() -> device_operation::set_resources()
 *	device_operation::set_resources() -> assign_resources()
 *
 * @param bus Pointer to the structure for this bus.
 */
void phase4_assign_resources(struct bus *bus)
{
	struct device *curdev;

	printk(BIOS_SPEW, "%s(%s) assign_resources, bus %d link: %d\n",
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
			       "%s(%s) ops has no missing phase4_set_resources\n",
			       curdev->dtsname, dev_path(curdev));
			continue;
		}
		curdev->ops->phase4_set_resources(curdev);
	}
	printk(BIOS_SPEW, "%s(%s) assign_resources, bus %d link: %d\n",
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
			"%s: dev %s: ops %p ops->phase2_setup_scan_bus %p\n",
			__FUNCTION__, dev->dtsname, dev->ops, 
			dev->ops? dev->ops->phase2_setup_scan_bus : NULL);
		if (dev->ops && dev->ops->phase2_setup_scan_bus) {
			printk(BIOS_SPEW,
			       "Calling phase2 phase2_setup_scan_bus...\n");
			dev->ops->phase2_setup_scan_bus(dev);
			printk(BIOS_SPEW, "phase2_setup_scan_bus done\n");
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
		printk(BIOS_INFO, "%s: %s: busdevice %p enabled %d ops %p\n",
		       __FUNCTION__, busdevice->dtsname, busdevice,
		       busdevice ? busdevice->enabled : 0,
		       busdevice ? busdevice->ops : NULL);
		printk(BIOS_INFO, "%s: can not scan from here, returning %d\n",
		       __FUNCTION__, max);
		return max;
	}

	if (busdevice->ops->phase3_enable_scan)
		busdevice->ops->phase3_enable_scan(busdevice);

	do_phase3 = 1;
	while (do_phase3) {
		int link;
		printk(BIOS_INFO, "%s: scanning %s(%s)\n", __FUNCTION__,
		       busdevice->dtsname, dev_path(busdevice));
		new_max = busdevice->ops->phase3_scan(busdevice, max);
		do_phase3 = 0;
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
 * Most of the devices in the system are listed in the mainboard Config.lb
 * file. The device structures for these devices are generated at compile
 * time by the config tool and are organized into the device tree. This
 * function determines if the devices created at compile time actually exist
 * in the physical system.
 * TODO: Fix comment, v3 doesn't have Config.lb files.
 *
 * For devices in the physical system but not listed in the Config.lb file,
 * the device structures have to be created at run time and attached to the
 * device tree.
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

	if (root->ops && root->ops->phase3_enable_scan) {
		root->ops->phase3_enable_scan(root);
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

/**
 * Configure devices on the device tree.
 * 
 * Starting at the root of the device tree, travel it recursively in two
 * passes. In the first pass, we compute and allocate resources (ranges)
 * required by each device. In the second pass, the resources ranges are
 * relocated to their final position and stored to the hardware.
 *
 * I/O resources start at DEVICE_IO_START and grow upward. MEM resources start
 * at DEVICE_MEM_START and grow downward.
 *
 * Since the assignment is hierarchical we set the values into the dev_root
 * struct. 
 */
void dev_phase4(void)
{
	struct resource *io, *mem;
	struct device *root;

	printk(BIOS_INFO, "Phase 4: Allocating resources...\n");

	root = &dev_root;
	if (!root->ops) {
		printk(BIOS_ERR,
		       "Phase 4: dev_root missing ops initialization\nPhase 4: Failed.\n");
		return;
	}
	if (!root->ops->phase4_read_resources) {
		printk(BIOS_ERR,
		       "dev_root ops missing read_resources\nPhase 4: Failed.\n");
		return;
	}

	if (!root->ops->phase4_set_resources) {
		printk(BIOS_ERR,
		       "dev_root ops missing set_resources\nPhase 4: Failed.\n");
		return;
	}

	printk(BIOS_INFO, "Phase 4: Reading resources...\n");
	root->ops->phase4_read_resources(root);
	printk(BIOS_INFO, "Phase 4: Done reading resources.\n");

	/* Get the resources. */
	io = &root->resource[0];
	mem = &root->resource[1];

	/* Make certain the I/O devices are allocated somewhere safe. */
	io->base = DEVICE_IO_START;
	io->flags |= IORESOURCE_ASSIGNED;
	io->flags &= ~IORESOURCE_STORED;

	/* Now reallocate the PCI resources memory with the
	 * highest addresses I can manage.
	 */
	mem->base = resource_max(&root->resource[1]);
	mem->flags |= IORESOURCE_ASSIGNED;
	mem->flags &= ~IORESOURCE_STORED;

#ifdef CONFIG_PCI_OPTION_ROM_RUN
	/* Allocate the VGA I/O resource. */
	allocate_vga_resource();
#endif

	/* Store the computed resource allocations into device registers. */
	printk(BIOS_INFO, "Phase 4: Setting resources...\n");
	root->ops->phase4_set_resources(root);
	printk(BIOS_INFO, "Phase 4: Done setting resources.\n");
#if 0
	mem->flags |= IORESOURCE_STORED;
	report_resource_stored(root, mem, "");
#endif

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
		if (dev->enabled && !dev->initialized &&
		    dev->ops && dev->ops->phase6_init) {
			if (dev->path.type == DEVICE_PATH_I2C) {
				printk(BIOS_DEBUG, "Phase 6: smbus: %s[%d]->",
				       dev_path(dev->bus->dev), dev->bus->link);
			}
			printk(BIOS_DEBUG, "Phase 6: %s init.\n",
			       dev_path(dev));
			dev->initialized = 1;
			dev->ops->phase6_init(dev);
		}
	}
	printk(BIOS_INFO, "Phase 6: Devices initialized.\n");
}

void show_all_devs(void)
{
	struct device *dev;

	printk(BIOS_INFO, "Show all devs...\n");
	for (dev = all_devices; dev; dev = dev->next) {
		printk(BIOS_SPEW,
		       "%s(%s): enabled %d have_resources %d initialized %d\n",
		       dev->dtsname, dev_path(dev), dev->enabled,
		       dev->have_resources, dev->initialized);
	}
}
