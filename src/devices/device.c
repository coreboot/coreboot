/*
 *      (c) 1999--2000 Martin Mares <mj@suse.cz>
 *      (c) 2003 Eric Biederman <ebiederm@xmission.com>
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

#include <console/console.h>
#include <bitops.h>
#include <device.h>
#include <arch/io.h>
#include <pci.h>

/**
 * This is the root of the device tree. A PCI tree always has 
 * one bus, bus 0. Bus 0 contains devices and bridges. 
 */
struct device dev_root;
/* Linked list of ALL devices */
struct device *all_devices = 0;
/* pointer to the last device */
static struct device **last_dev_p = &all_devices;

#define DEVICE_MEM_HIGH  0xFEC00000UL /* Reserve 20M for the system */
#define DEVICE_IO_START 0x1000


unsigned long device_memory_base;


/* Append a new device to the global device chain.
 * The chain is used to find devices once everything is set up.
 */
void append_device(struct device *dev)
{
	*last_dev_p = dev;
	last_dev_p = &dev->next;
}


/** round a number to an alignment. 
 * @param val the starting value
 * @param roundup Alignment as a power of two
 * @returns rounded up number
 */
static unsigned long round(unsigned long val, unsigned long roundup)
{
	/* ROUNDUP MUST BE A POWER OF TWO. */
	unsigned long inverse;
	inverse = ~(roundup - 1);
	val += (roundup - 1);
	val &= inverse;
	return val;
}

static unsigned long round_down(unsigned long val, unsigned long round_down)
{
	/* ROUND_DOWN MUST BE A POWER OF TWO. */
	unsigned long inverse;
	inverse = ~(round_down - 1);
	val &= inverse;
	return val;
}


/** Read the resources on all devices of a given bus.
 * @param bus bus to read the resources on.
 */
static void read_resources(struct device *bus)
{
	struct device *curdev;

	
	/* Walk through all of the devices and find which resources they need. */
	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (curdev->resources > 0) {
			continue;
		}
		curdev->ops->read_resources(curdev);
	}
}

static struct device *largest_resource(struct device *bus, struct resource **result_res,
	unsigned long type_mask, unsigned long type)
{
	struct device *curdev;
	struct device *result_dev = 0;
	struct resource *last = *result_res;
	struct resource *result = 0;
	int seen_last = 0;
	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		int i;
		for(i = 0; i < curdev->resources; i++) {
			struct resource *resource = &curdev->resource[i];
			/* If it isn't the right kind of resource ignore it */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			/* Be certain to pick the successor to last */
			if (resource == last) {
				seen_last = 1;
				continue;
			}
			if (last && (
				(last->align < resource->align) ||
				((last->align == resource->align) &&
					(last->size < resource->size)) ||
				((last->align == resource->align) &&
					(last->size == resource->size) &&
					(!seen_last)))) {
				continue;
			}
			if (!result || 
				(result->align < resource->align) ||
				((result->align == resource->align) &&
					(result->size < resource->size))) {
				result_dev = curdev;
				result = resource;
			}
		}
	}
	*result_res = result;
	return result_dev;
}

/* Compute allocate resources is the guts of the resource allocator.
 * 
 * The problem.
 *  - Allocate resources locations for every device.
 *  - Don't overlap, and follow the rules of bridges.
 *  - Don't overlap with resources in fixed locations.
 *  - Be efficient so we don't have ugly strategies.
 *
 * The strategy.
 * - Devices that have fixed addresses are the minority so don't
 *   worry about them too much.  Instead only use part of the address
 *   space for devices with programmable addresses.  This easily handles
 *   everything except bridges.
 *
 * - PCI devices are required to have thier sizes and their alignments
 *   equal.  In this case an optimal solution to the packing problem
 *   exists.  Allocate all devices from highest alignment to least
 *   alignment or vice versa.  Use this.
 *
 * - So we can handle more than PCI run two allocation passes on
 *   bridges.  The first to see how large the resources are behind
 *   the bridge, and what their alignment requirements are.  The
 *   second to assign a safe address to the devices behind the
 *   bridge.  This allows me to treat a bridge as just a device with 
 *   a couple of resources, and not need to special case it in the
 *   allocator.  Also this allows handling of other types of bridges.
 *
 */

void compute_allocate_resource(
	struct device *bus,
	struct resource *bridge,
	unsigned long type_mask,
	unsigned long type)
{
	struct device *dev;
	struct resource *resource;
	unsigned long base;
	unsigned long align, min_align;
	min_align = 0;
	base = bridge->base;

	/* We want different minimum alignments for different kinds of
	 * resources.  These minimums are not device type specific
	 * but resource type specific.
	 */
	if (bridge->flags & IORESOURCE_IO) {
		min_align = log2(DEVICE_IO_ALIGN);
	}
	if (bridge->flags & IORESOURCE_MEM) {
		min_align = log2(DEVICE_MEM_ALIGN);
	}

	printk_spew("DEV: %02x:%02x.%01x compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d\n", 
		bus->bus->secondary,
		PCI_SLOT(bus->devfn), PCI_FUNC(bus->devfn),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);

	/* Make certain I have read in all of the resources */
	read_resources(bus);

	/* Remember I haven't found anything yet. */
	resource = 0;

	/* Walk through all the devices on the current bus and compute the addresses */
	while((dev = largest_resource(bus, &resource, type_mask, type))) {
		unsigned long size;
		/* Do NOT I repeat do not ignore resources which have zero size.
		 * If they need to be ignored dev->read_resources should not even
		 * return them.   Some resources must be set even when they have
		 * no size.  PCI bridge resources are a good example of this.
		 */

		/* Propogate the resource alignment to the bridge register  */
		if (resource->align > bridge->align) {
			bridge->align = resource->align;
		}

		/* Make certain we are dealing with a good minimum size */
		size = resource->size;
		align = resource->align;
		if (align < min_align) {
			align = min_align;
		}
		if (resource->flags & IORESOURCE_IO) {
			/* Don't allow potential aliases over the
			 * legacy pci expansion card addresses.
			 */
			if ((base > 0x3ff) && ((base & 0x300) != 0)) {
				base = (base & ~0x3ff) + 0x400;
			}
			/* Don't allow allocations in the VGA IO range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}
		if (((round(base, 1UL << align) + size) -1) <= resource->limit) {
			/* base must be aligned to size */
			base = round(base, 1UL << align);
			resource->base = base;
			resource->flags |= IORESOURCE_SET;
			base += size;
			
			printk_spew(
				"DEV: %02x:%02x.%01x %02x *  [0x%08lx - 0x%08lx] %s\n",
				dev->bus->secondary, 
				PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
				resource->index, 
				resource->base, resource->base + resource->size -1,
				(resource->flags & IORESOURCE_IO)? "io":
				(resource->flags & IORESOURCE_PREFETCH)? "prefmem": "mem");
		}

	}
	/* A pci bridge resource does not need to be a power
	 * of two size, but it does have a minimum granularity.
	 * Round the size up to that minimum granularity so we
	 * know not to place something else at an address postitively
	 * decoded by the bridge.
	 */
	bridge->size = round(base, 1UL << bridge->gran) - bridge->base;

	printk_spew("DEV: %02x:%02x.%01x compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d done\n", 
		bus->bus->secondary,
		PCI_SLOT(bus->devfn), PCI_FUNC(bus->devfn),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);


}

static void allocate_vga_resource(void)
{
	/* FIXME handle the VGA pallette snooping */
	struct device *dev, *vga, *bus;
	bus = vga = 0;
	for(dev = all_devices; dev; dev = dev->next) {
		uint32_t class_revision;
		pci_read_config_dword(dev, PCI_CLASS_REVISION, &class_revision);
		if (((class_revision >> 24) == 0x03) && 
		    ((class_revision >> 16) != 0x380)) {
			if (!vga) {
				printk_debug("Allocating VGA resource\n");
				vga = dev;
			}
			if (vga == dev) {
				/* All legacy VGA cards have MEM & I/O space registers */
				dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
			} else {
				/* It isn't safe to enable other VGA cards */
				dev->command &= ~(PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
			}
		}
	}
	if (vga) {
		bus = vga->bus;
	}
	/* Now walk up the bridges setting the VGA enable */
	while(bus) {
		uint16_t ctrl;
		pci_read_config_word(bus, PCI_BRIDGE_CONTROL, &ctrl);
		ctrl |= PCI_BRIDGE_CTL_VGA;
		pci_write_config_word(bus, PCI_BRIDGE_CONTROL, ctrl);
		bus = (bus == bus->bus)? 0 : bus->bus;
	} 
}


/** Assign the computed resources to the bridges and devices on the bus.
 * Recurse to any bridges found on this bus first. Then do the devices
 * on this bus. 
 * @param bus Pointer to the structure for this bus
 */ 
void assign_resources(struct device *bus)
{
	struct device *curdev;

	printk_debug("ASSIGN RESOURCES, bus %d\n", bus->secondary);

	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		curdev->ops->set_resources(curdev);
	}
	printk_debug("ASSIGNED RESOURCES, bus %d\n", bus->secondary);
}

static void enable_resources(struct device *bus)
{
	struct device *curdev;

	/* Walk through the chain of all pci devices and enable them.
	 * This is effectively a breadth first traversal so we should
	 * not have enalbing ordering problems.
	 */
	for (curdev = all_devices; curdev; curdev = curdev->next) {
		uint16_t command;
		pci_read_config_word(curdev, PCI_COMMAND, &command);
		command |= curdev->command;
		printk_debug("DEV: %02x:%02x.%01x cmd <- %02x\n",
			curdev->bus->secondary,
			PCI_SLOT(curdev->devfn), PCI_FUNC(curdev->devfn),
			command);
		pci_write_config_word(curdev, PCI_COMMAND, command);
	}
}

/** Enumerate the resources on the PCI by calling pci_init
 */
void dev_enumerate(void)
{
	struct device *root;
	printk_info("Enumerating buses...");
	root = &dev_root;
	if (!root->ops) {
		root->ops = &default_pci_ops_root;
	}
	root->subordinate = root->ops->scan_bus(root, 0);
	printk_info("done\n");
}

/** Starting at the root, compute what resources are needed and allocate them. 
 * I/O starts at PCI_IO_START. Since the assignment is hierarchical we
 * set the values into the dev_root struct. 
 */
void dev_configure(void)
{
	struct device *root = &dev_root;
	printk_info("Allocating resources...");
	printk_debug("\n");


	root->ops->read_resources(root);

	/* Make certain the io devices are allocated somewhere
	 * safe.
	 */
	root->resource[0].base = DEVICE_IO_START;
	root->resource[0].flags |= IORESOURCE_SET;
	/* Now reallocate the pci resources memory with the
	 * highest addresses I can manage.
	 */
	root->resource[1].base = 
		round_down(DEVICE_MEM_HIGH - root->resource[1].size,
			1UL << root->resource[1].align);
	device_memory_base = root->resource[1].base;
	root->resource[1].flags |= IORESOURCE_SET;
	// now just set things into registers ... we hope ...
	root->ops->set_resources(root);

	allocate_vga_resource();

	printk_info("done.\n");
}

/** Starting at the root, walk the tree and enable all devices/bridges. 
 * What really happens is computed COMMAND bits get set in register 4
 */
void dev_enable(void)
{
	printk_info("Enabling resourcess...");

	/* now enable everything. */
	enable_resources(&dev_root);
	printk_info("done.\n");
}

/** Starting at the root, walk the tree and call a driver to
 *  do device specific setup.
 */
void dev_initialize(void)
{
	struct device *dev;

	printk_info("Initializing devices...\n");
	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops->init) {
			printk_debug("PCI: %02x:%02x.%01x init\n",
				dev->bus->secondary,
				PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
			dev->ops->init(dev);
		}
	}
	printk_info("Devices initialized\n");
}


