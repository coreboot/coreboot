/*
 *      PCI Bus Services, see include/linux/pci.h for further explanation.
 *
 *      Copyright 1993 -- 1997 Drew Eckhardt, Frederic Potter,
 *      David Mosberger-Tang
 *
 *      Copyright 1997 -- 1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *	
 *	Copyright 2003 -- Eric Biederman <ebiederman@lnxi.com>
 */

#include <console/console.h>
#include <stdlib.h>
#include <stdint.h>
#include <bitops.h>
#include <pci.h>
#include <pci_ids.h>
#include <string.h>

static unsigned int pci_scan_bridge(struct device *bus, unsigned int max);

/** Given a device and register, read the size of the BAR for that register. 
 * @param dev       Pointer to the device structure
 * @param resource  Pointer to the resource structure
 * @param index     Address of the pci configuration register
 */
static void pci_get_resource(struct device *dev, struct resource *resource, unsigned long index)
{
	uint32_t addr, size, base;
	unsigned long type;

	/* Initialize the resources to nothing */
	resource->base = 0;
	resource->size = 0;
	resource->align = 0;
	resource->gran = 0;
	resource->limit = 0;
	resource->flags = 0;
	resource->index = index;

	pci_read_config_dword(dev, index, &addr);
	if (addr == 0xffffffffUL)
		return;

	/* FIXME: more consideration for 64-bit PCI devices,
	 * we currently detect their size but otherwise
	 * treat them as 32-bit resources
	 */
	/* get the size */
	pci_write_config_dword(dev, index, ~0);
	pci_read_config_dword(dev,  index, &size);

	/* get the minimum value the bar can be set to */
	pci_write_config_dword(dev, index, 0);
	pci_read_config_dword(dev, index, &base);

	/* restore addr */
	pci_write_config_dword(dev, index, addr);

	/*
	 * some broken hardware has read-only registers that do not 
	 * really size correctly. You can tell this if addr == size
	 * Example: the acer m7229 has BARs 1-4 normally read-only. 
	 * so BAR1 at offset 0x10 reads 0x1f1. If you size that register
	 * by writing 0xffffffff to it, it will read back as 0x1f1 -- a 
	 * violation of the spec. 
	 * We catch this case and ignore it by settting size and type to 0.
	 * This incidentally catches the common case where registers 
	 * read back as 0 for both address and size. 
	 */
	if ((addr == size) && (addr == base)) {
		if (size != 0) {
			printk_debug(
				"PCI: %02x:%02x.%01x register %02x(%08x), read-only ignoring it\n",
				dev->bus->secondary,
				PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
				index, addr);
		}
		resource->flags = 0;
	}
	/* Now compute the actual size, See PCI Spec 6.2.5.1 ...  */
	else if (size & PCI_BASE_ADDRESS_SPACE_IO) {
		type = size & (~PCI_BASE_ADDRESS_IO_MASK);
		/* BUG! Top 16 bits can be zero (or not) 
		 * So set them to 0xffff so they go away ...
		 */
		resource->size = (~((size | 0xffff0000) & PCI_BASE_ADDRESS_IO_MASK)) +1;
		resource->align = log2(resource->size);
		resource->gran = resource->align;
		resource->flags = IORESOURCE_IO;
		resource->limit = 0xffff;
	} 
	else {
		/* A Memory mapped base address */
		type = size & (~PCI_BASE_ADDRESS_MEM_MASK);
		resource->size = (~(size &PCI_BASE_ADDRESS_MEM_MASK)) +1;
		resource->align = log2(resource->size);
		resource->gran = resource->align;
		resource->flags = IORESOURCE_MEM;
		if (type & PCI_BASE_ADDRESS_MEM_PREFETCH) {
			resource->flags |= IORESOURCE_PREFETCH;
		}
		type &= PCI_BASE_ADDRESS_MEM_TYPE_MASK;
		if (type == PCI_BASE_ADDRESS_MEM_TYPE_32) {
			/* 32bit limit */
			resource->limit = 0xffffffffUL;
		}
		else if (type == PCI_BASE_ADDRESS_MEM_TYPE_1M) {
			/* 1MB limit */
			resource->limit = 0x000fffffUL;
		}
		else if (type == PCI_BASE_ADDRESS_MEM_TYPE_64) {
			unsigned long index_hi;
			/* 64bit limit 
			 * For now just treat this as a 32bit limit
			 */
			index_hi = index + 4;
			resource->limit = 0xffffffffUL;
			resource->flags |= IORESOURCE_PCI64;
			pci_read_config_dword( dev, index_hi, &addr);
			/* get the extended size */
			pci_write_config_dword(dev, index_hi, 0xffffffffUL);
			pci_read_config_dword( dev, index_hi, &size);

			/* get the minimum value the bar can be set to */
			pci_write_config_dword(dev, index_hi, 0);
			pci_read_config_dword(dev,  index_hi, &base);

			/* restore addr */
			pci_write_config_dword(dev, index_hi, addr);
			
			if ((size == 0xffffffff) && (base == 0)) {
				/* Clear the top half of the bar */
				pci_write_config_dword(dev, index_hi, 0);
			}
			else {
				printk_err("PCI: %02x:%02x.%01x Unable to handle 64-bit address\n",
					dev->bus->secondary, 
					PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
				resource->flags = IORESOURCE_PCI64;
			}
		} 
		else {
			/* Invalid value */
			resource->flags = 0;
		}
	}
	/* dev->size holds the flags... */
	return;
}

/** Read the base address registers for a given device. 
 * @param dev Pointer to the dev structure
 * @param howmany How many registers to read (6 for device, 2 for bridge)
 */
static void pci_read_bases(struct device *dev, unsigned int howmany)
{
	unsigned int reg;
	unsigned long index;

	reg = dev->resources;
	for(index = PCI_BASE_ADDRESS_0; 
	    (reg < MAX_RESOURCES) && (index < PCI_BASE_ADDRESS_0 + (howmany << 2)); ) {
		struct resource *resource;
		resource = &dev->resource[reg];
		pci_get_resource(dev, resource, index);
		reg += (resource->flags & (IORESOURCE_IO | IORESOURCE_MEM))? 1:0;
		index += (resource->flags & IORESOURCE_PCI64)?8:4;
	}
	dev->resources = reg;
}


static void pci_bridge_read_bases(struct device *dev)
{
	unsigned int reg = dev->resources;

	/* FIXME handle bridges without some of the optional resources */

	/* Initialize the io space constraints on the current bus */
	dev->resource[reg].base  = 0;
	dev->resource[reg].size  = 0;
	dev->resource[reg].align = log2(PCI_IO_BRIDGE_ALIGN);
	dev->resource[reg].gran  = log2(PCI_IO_BRIDGE_ALIGN);
	dev->resource[reg].limit = 0xffffUL;
	dev->resource[reg].flags = IORESOURCE_IO | IORESOURCE_PCI_BRIDGE;
	dev->resource[reg].index = PCI_IO_BASE;
	compute_allocate_resource(dev, &dev->resource[reg],
		IORESOURCE_IO, IORESOURCE_IO);
	reg++;

	/* Initiliaze the prefetchable memory constraints on the current bus */
	dev->resource[reg].base = 0;
	dev->resource[reg].size = 0;
	dev->resource[reg].align = log2(PCI_MEM_BRIDGE_ALIGN);
	dev->resource[reg].gran  = log2(PCI_MEM_BRIDGE_ALIGN);
	dev->resource[reg].limit = 0xffffffffUL;
	dev->resource[reg].flags = IORESOURCE_MEM | IORESOURCE_PREFETCH | IORESOURCE_PCI_BRIDGE;
	dev->resource[reg].index = PCI_PREF_MEMORY_BASE;
	compute_allocate_resource(dev, &dev->resource[reg],
		IORESOURCE_MEM | IORESOURCE_PREFETCH, 
		IORESOURCE_MEM | IORESOURCE_PREFETCH);
	reg++;

	/* Initialize the memory resources on the current bus */
	dev->resource[reg].base = 0;
	dev->resource[reg].size = 0;
	dev->resource[reg].align = log2(PCI_MEM_BRIDGE_ALIGN);
	dev->resource[reg].gran  = log2(PCI_MEM_BRIDGE_ALIGN);
	dev->resource[reg].limit = 0xffffffffUL;
	dev->resource[reg].flags = IORESOURCE_MEM | IORESOURCE_PCI_BRIDGE;
	dev->resource[reg].index = PCI_MEMORY_BASE;
	compute_allocate_resource(dev, &dev->resource[reg],
		IORESOURCE_MEM | IORESOURCE_PREFETCH, 
		IORESOURCE_MEM);
	reg++;

	dev->resources = reg;
}


static void pci_dev_read_resources(struct device *dev)
{
	uint32_t addr;
	dev->resources = 0;
	memset(&dev->resource[0], 0, sizeof(dev->resource));
	pci_read_bases(dev, 6);
	pci_read_config_dword(dev, PCI_ROM_ADDRESS, &addr);
	dev->rom_address = (addr == 0xffffffff)? 0 : addr;
}

static void pci_bus_read_resources(struct device *dev)
{
	uint32_t addr;
	dev->resources = 0;
	memset(&dev->resource[0], 0, sizeof(dev->resource));
	pci_bridge_read_bases(dev);
	pci_read_bases(dev, 2);
	
	pci_read_config_dword(dev, PCI_ROM_ADDRESS1, &addr);
	dev->rom_address = (addr == 0xffffffff)? 0 : addr;

}


static void pci_set_resource(struct device *dev, struct resource *resource)
{
	unsigned long base, limit;
	unsigned long bridge_align = PCI_MEM_BRIDGE_ALIGN;
	unsigned char buf[10];
	
	/* Make certain the resource has actually been set */
	if (!(resource->flags & IORESOURCE_SET)) {
#if 1
		printk_err("ERROR: %02x:%02x.%01x %02x not allocated\n",
			dev->bus->secondary,
			PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
			resource->index);
#endif
		return;
	}

	/* Only handle PCI memory and IO resources for now */
	if (!(resource->flags & (IORESOURCE_MEM |IORESOURCE_IO)))
		return;
	
	if (resource->flags & IORESOURCE_MEM) {
		dev->command |= PCI_COMMAND_MEMORY;
		bridge_align = PCI_MEM_BRIDGE_ALIGN;
	}
	if (resource->flags & IORESOURCE_IO) {
		dev->command |= PCI_COMMAND_IO;
		bridge_align = PCI_IO_BRIDGE_ALIGN;
	}
	if (resource->flags & IORESOURCE_PCI_BRIDGE) {
		dev->command |= PCI_COMMAND_MASTER;
	}
	/* Get the base address */
	base = resource->base;
	
	/* Get the limit (rounded up) */
	limit = base + ((resource->size + bridge_align - 1UL) & ~(bridge_align -1)) -1UL;
	
	if (!(resource->flags & IORESOURCE_PCI_BRIDGE)) {
		/*
		 * some chipsets allow us to set/clear the IO bit. 
		 * (e.g. VIA 82c686a.) So set it to be safe)
		 */
		limit = base + resource->size -1;
		if (resource->flags & IORESOURCE_IO) {
			base |= PCI_BASE_ADDRESS_SPACE_IO;
		}
		pci_write_config_dword(dev, resource->index, base & 0xffffffff);
		if (resource->flags & IORESOURCE_PCI64) {
			/* FIXME handle real 64bit base addresses */
			pci_write_config_dword(dev, resource->index + 4, 0);
		}
	}
	else if (resource->index == PCI_IO_BASE) {
		/* set the IO ranges
		 * WARNING: we don't really do 32-bit addressing for IO yet! 
		 */
		compute_allocate_resource(dev, resource, 
			IORESOURCE_IO, IORESOURCE_IO);
		pci_write_config_byte(dev, PCI_IO_BASE,  base >> 8);
		pci_write_config_byte(dev, PCI_IO_LIMIT, limit >> 8);
	}
	else if (resource->index == PCI_MEMORY_BASE) {
		/* set the memory range
		 */
		compute_allocate_resource(dev, resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH, 
			IORESOURCE_MEM);
		pci_write_config_word(dev, PCI_MEMORY_BASE, base >> 16);
		pci_write_config_word(dev, PCI_MEMORY_LIMIT, limit >> 16);
	}
	else if (resource->index == PCI_PREF_MEMORY_BASE) {
		/* set the prefetchable memory range
		 * WARNING: we don't really do 64-bit addressing for prefetchable memory yet!
		 */
		compute_allocate_resource(dev, resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH, 
			IORESOURCE_MEM | IORESOURCE_PREFETCH);
		pci_write_config_word(dev, PCI_PREF_MEMORY_BASE,  base >> 16);
		pci_write_config_word(dev, PCI_PREF_MEMORY_LIMIT, limit >> 16);
	}
	else {
		printk_err("ERROR: invalid resource->index %x\n",
			resource->index);
	}
	buf[0] = '\0';
	if (resource->flags & IORESOURCE_PCI_BRIDGE) {
		sprintf(buf, "bus %d ", dev->secondary);
	}
	
	printk_debug(
		"PCI: %02x:%02x.%01x %02x <- [0x%08lx - 0x%08lx] %s%s\n",
		dev->bus->secondary, 
		PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
		resource->index, 
		resource->base, limit,
		buf,
		(resource->flags & IORESOURCE_IO)? "io":
		(resource->flags & IORESOURCE_PREFETCH)? "prefmem": "mem");
	return;
}

static void pci_dev_set_resources(struct device *dev)
{
	struct resource *resource, *last;
	uint8_t line;

	last = &dev->resource[dev->resources];

	for(resource = &dev->resource[0]; resource < last; resource++) {
		pci_set_resource(dev, resource);
	}
	if (dev->children) {
		assign_resources(dev);
	}

	/* set a default latency timer */
	pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0x40);

	/* set a default secondary latency timer */
	if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE) {
		pci_write_config_byte(dev, PCI_SEC_LATENCY_TIMER, 0x40);
	}

	/* zero the irq settings */
	pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &line);
	if (line) {
		pci_write_config_byte(dev, PCI_INTERRUPT_LINE, 0);
	}
	/* set the cache line size, so far 64 bytes is good for everyone */
	pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE, 64 >> 2);
}

struct device_operations default_pci_ops_dev = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.init = 0,
	.scan_bus = 0,
};
struct device_operations default_pci_ops_bus = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.init = 0,
	.scan_bus = pci_scan_bridge,
};
static void set_pci_ops(struct device *dev)
{
	struct pci_driver *driver;
	if (dev->ops) {
		return;
	}
	/* Look through the list of setup drivers and find one for
	 * this pci device 
	 */
	for(driver = &pci_drivers[0]; driver != &epci_drivers[0]; driver++) {
		if ((driver->vendor == dev->vendor) &&
			(driver->device = dev->device)) {
			dev->ops = driver->ops;
			break;
		}
	}
	/* If I don't have a specific driver use the default operations */
	switch(dev->hdr_type & 0x7f) {	/* header type */
	case PCI_HEADER_TYPE_NORMAL:	/* standard header */
		if ((dev->class >> 8) == PCI_CLASS_BRIDGE_PCI)
			goto bad;
		dev->ops = &default_pci_ops_dev;
		break;
	case PCI_HEADER_TYPE_BRIDGE:
		if ((dev->class >> 8) != PCI_CLASS_BRIDGE_PCI)
			goto bad;
		dev->ops = &default_pci_ops_bus;
		break;
	default:
	bad:
		printk_err("PCI: %02x:%02x.%01x [%04x/%04x/%06x] has unknown header "
			"type %02x, ignoring.\n",
			dev->bus->secondary, 
			PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
			dev->vendor, dev->device, 
			dev->class >> 8, dev->hdr_type);
	}
	return;
}

/**
 * Given a bus and a devfn number, find the device structure
 * @param bus The bus structure
 * @param devfn a device/function number
 * @return pointer to the device structure
 */
static struct device *pci_scan_get_dev(struct device **list, unsigned int devfn)
{
	struct device *dev = 0;
	for(; *list; list = &(*list)->sibling) {
		if ((*list)->devfn == devfn) {
			/* Unlink from the list */
			dev = *list;
			*list = (*list)->sibling;
			dev->sibling = 0;
			break;
		}
	}
	return dev;
}


/** Scan the pci bus devices and bridges.
 * @param pci_bus pointer to the bus structure
 * @param max current bus number
 * @return The maximum bus number found, after scanning all subordinate busses
 */
unsigned int pci_scan_bus(struct device *bus, unsigned int max)
{
	unsigned int devfn;
	struct device *dev, **bus_last;
	struct device *old_devices;
	struct device *child;

	printk_debug("PCI: pci_scan_bus for bus %d\n", bus->secondary);

	old_devices = bus->children;
	bus->children = 0;
	bus_last = &bus->children;

	post_code(0x24);
	

	/* probe all devices on this bus with some optimization for non-existance and 
	   single funcion devices */
	for (devfn = 0; devfn < 0xff; devfn++) {
		struct device dummy;
		uint32_t id, class;
		uint8_t cmd, tmp, hdr_type;

		/* First thing setup the device structure */
		dev = pci_scan_get_dev(&old_devices, devfn);
	
		dummy.bus = bus;
		dummy.devfn = devfn;
		pci_read_config_dword(&dummy, PCI_VENDOR_ID, &id);
		/* some broken boards return 0 if a slot is empty: */
		if (!dev &&
			(id == 0xffffffff || id == 0x00000000 || 
			 id == 0x0000ffff || id == 0xffff0000)) {
			printk_spew("PCI: devfn 0x%x, bad id 0x%x\n", devfn, id);
			if (PCI_FUNC(devfn) == 0x00) {
				/* if this is a function 0 device and it is not present,
				   skip to next device */
				devfn += 0x07;
			}
			/* multi function device, skip to next function */
			continue;
		}
		pci_read_config_byte(&dummy, PCI_HEADER_TYPE, &hdr_type);
		pci_read_config_dword(&dummy, PCI_CLASS_REVISION, &class);

		if (!dev) {
			if ((dev = malloc(sizeof(*dev))) == 0) {
				printk_err("PCI: out of memory.\n");
				continue;
			}
			memset(dev, 0, sizeof(*dev));
		}

		dev->bus = bus;
		dev->devfn = devfn;
		dev->vendor = id & 0xffff;
		dev->device = (id >> 16) & 0xffff;
		dev->hdr_type = hdr_type;
		/* class code, the upper 3 bytes of PCI_CLASS_REVISION */
		dev->class = class >> 8;

		/* non-destructively determine if device can be a master: */
		pci_read_config_byte(dev, PCI_COMMAND, &cmd);
		pci_write_config_byte(dev, PCI_COMMAND, cmd | PCI_COMMAND_MASTER);
		pci_read_config_byte(dev, PCI_COMMAND, &tmp);

		dev->master = ((tmp & PCI_COMMAND_MASTER) != 0);
		pci_write_config_byte(dev, PCI_COMMAND, cmd);

		/* Look at the vendor and device id, or at least the 
		 * header type and class and figure out which set of configuration
		 * methods to use.
		 */
		set_pci_ops(dev);
		/* Kill the device if we don't have some pci operations for it */
		if (!dev->ops) {
			free(dev);
			continue;
		}
		printk_debug("PCI: %02x:%02x.%01x [%04x/%04x]\n", 
			bus->secondary, PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
			dev->vendor, dev->device);

		/* Put it into the global device chain. */
		append_device(dev);

		/* Now insert it into the list of devices held by the parent bus. */
		*bus_last = dev;
		bus_last = &dev->sibling;

		if (PCI_FUNC(devfn) == 0x00 && (hdr_type & 0x80) != 0x80) {
			/* if this is not a multi function device, don't waste time probe
			   another function. Skip to next device. */
			devfn += 0x07;
		}
	}
	post_code(0x25);

	for(child = bus->children; child; child = child->sibling) {
		if (!child->ops->scan_bus)
			continue;
		max = child->ops->scan_bus(child, max);
		
	}
	/*
	 * We've scanned the bus and so we know all about what's on
	 * the other side of any bridges that may be on this bus plus
	 * any devices.
	 *
	 * Return how far we've got finding sub-buses.
	 */
	printk_debug("PCI: pci_scan_bus returning with max=%02x\n", max);
	post_code(0x55);
	return max;
}

/** Scan the bus, first for bridges and next for devices. 
 * @param pci_bus pointer to the bus structure
 * @return The maximum bus number found, after scanning all subordinate busses
 */
static unsigned int pci_scan_bridge(struct device *bus, unsigned int max)
{
	uint32_t buses;
	uint16_t cr;
	/* Set up the primary, secondary and subordinate bus numbers. We have
	 * no idea how many buses are behind this bridge yet, so we set the
	 * subordinate bus number to 0xff for the moment 
	 */
	bus->secondary = ++max;
	bus->subordinate = 0xff;
	
	/* Clear all status bits and turn off memory, I/O and master enables. */
	pci_read_config_word(bus, PCI_COMMAND, &cr);
	pci_write_config_word(bus, PCI_COMMAND, 0x0000);
	pci_write_config_word(bus, PCI_STATUS, 0xffff);

	/*
	 * Read the existing primary/secondary/subordinate bus
	 * number configuration.
	 */
	pci_read_config_dword(bus, PCI_PRIMARY_BUS, &buses);

	/* Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagated by the bridge if it is not
	 * correctly configured 
	 */
	buses &= 0xff000000;
	buses |= (((unsigned int) (bus->bus->secondary) << 0) |
		((unsigned int) (bus->secondary) << 8) |
		((unsigned int) (bus->subordinate) << 16));
	pci_write_config_dword(bus, PCI_PRIMARY_BUS, buses);
	
	/* Now we can scan all subordinate buses i.e. the bus hehind the bridge */
	max = pci_scan_bus(bus, max);
	
	/* We know the number of buses behind this bridge. Set the subordinate
	 *  bus number to its real value 
	 */
	bus->subordinate = max;
	buses = (buses & 0xff00ffff) |
		((unsigned int) (bus->subordinate) << 16);
	pci_write_config_dword(bus, PCI_PRIMARY_BUS, buses);
	pci_write_config_word(bus, PCI_COMMAND, cr);
		
	return max;
}


static void pci_root_read_resources(struct device *bus)
{
	int res = 0;
	/* Initialize the system wide io space constraints */
	bus->resource[res].base  = 0x400;
	bus->resource[res].size  = 0;
	bus->resource[res].align = 0;
	bus->resource[res].gran  = 0;
	bus->resource[res].limit = 0xffffUL;
	bus->resource[res].flags = IORESOURCE_IO;
	bus->resource[res].index = PCI_IO_BASE;
	compute_allocate_resource(bus, &bus->resource[res], 
		IORESOURCE_IO, IORESOURCE_IO);
	res++;

	/* Initialize the system wide memory resources constraints */
	bus->resource[res].base  = 0;
	bus->resource[res].size  = 0;
	bus->resource[res].align = 0;
	bus->resource[res].gran  = 0;
	bus->resource[res].limit = 0xffffffffUL;
	bus->resource[res].flags = IORESOURCE_MEM;
	bus->resource[res].index = PCI_MEMORY_BASE;
	compute_allocate_resource(bus, &bus->resource[res], 
		IORESOURCE_MEM, IORESOURCE_MEM);
	res++;

	bus->resources = res;
}
static void pci_root_set_resources(struct device *bus)
{
	compute_allocate_resource(bus,
		&bus->resource[0], IORESOURCE_IO, IORESOURCE_IO);
	compute_allocate_resource(bus, 
		&bus->resource[1], IORESOURCE_MEM, IORESOURCE_MEM);
	assign_resources(bus);
}

struct device_operations default_pci_ops_root = {
	.read_resources = pci_root_read_resources,
	.set_resources = pci_root_set_resources,
	.init = 0,
	.scan_bus = pci_scan_bus,
};

