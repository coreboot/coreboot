/*
 *    Low-Level PCI Support for PC
 *
 *      (c) 1999--2000 Martin Mares <mj@suse.cz>
 */
/* lots of mods by ron minnich (rminnich@lanl.gov), with 
 * the final architecture guidance from Tom Merritt (tjm@codegen.com)
 * In particular, we changed from the one-pass original version to 
 * Tom's recommended multiple-pass version. I wasn't sure about doing 
 * it with multiple passes, until I actually started doing it and saw
 * the wisdom of Tom's recommendations ...
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <bitops.h>
#include <pci.h>
#undef __KERNEL__
#include <arch/io.h>
#include <printk.h>


#define PCI_MEM_HIGH  0xFEC00000UL /* Reserve 20M for the system */
#define PCI_MEM_START 0xC0000000
#define PCI_IO_START 0x1000

/* PCI_ALLOCATE_TIGHT selects a near optimal pci resource allocate
 * algorithm, in case there are problems set this to 0 and scream at
 * me: Eric Biederman <ebiederman@lnxi.com>
 */
#define PCI_ALLOCATE_TIGHT 1

static const struct pci_ops *conf;

struct pci_ops {
	int (*read_byte) (u8 bus, int devfn, int where, u8 * val);
	int (*read_word) (u8 bus, int devfn, int where, u16 * val);
	int (*read_dword) (u8 bus, int devfn, int where, u32 * val);
	int (*write_byte) (u8 bus, int devfn, int where, u8 val);
	int (*write_word) (u8 bus, int devfn, int where, u16 val);
	int (*write_dword) (u8 bus, int devfn, int where, u32 val);
};


/*
 * Direct access to PCI hardware...
 */


/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))

static int pci_conf1_read_config_byte(unsigned char bus, int devfn, int where, u8 * value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	*value = inb(0xCFC + (where & 3));
	return 0;
}

static int pci_conf1_read_config_word(unsigned char bus, int devfn, int where, u16 * value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	*value = inw(0xCFC + (where & 2));
	return 0;
}

static int pci_conf1_read_config_dword(unsigned char bus, int devfn, int where, u32 * value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	*value = inl(0xCFC);
	return 0;
}

static int pci_conf1_write_config_byte(unsigned char bus, int devfn, int where, u8 value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outb(value, 0xCFC + (where & 3));
	return 0;
}

static int pci_conf1_write_config_word(unsigned char bus, int devfn, int where, u16 value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outw(value, 0xCFC + (where & 2));
	return 0;
}

static int pci_conf1_write_config_dword(unsigned char bus, int devfn, int where, u32 value)
{
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outl(value, 0xCFC);
	return 0;
}

#undef CONFIG_CMD

static const struct pci_ops pci_direct_conf1 =
{
	pci_conf1_read_config_byte,
	pci_conf1_read_config_word,
	pci_conf1_read_config_dword,
	pci_conf1_write_config_byte,
	pci_conf1_write_config_word,
	pci_conf1_write_config_dword
};

/*
 * Functions for accessing PCI configuration space with type 2 accesses
 */

#define IOADDR(devfn, where)	((0xC000 | ((devfn & 0x78) << 5)) + where)
#define FUNC(devfn)		(((devfn & 7) << 1) | 0xf0)
#define SET(bus,devfn)		if (devfn & 0x80) return -1;outb(FUNC(devfn), 0xCF8); outb(bus, 0xCFA);

static int pci_conf2_read_config_byte(unsigned char bus, int devfn, int where, u8 * value)
{
	SET(bus, devfn);
	*value = inb(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return 0;
}

static int pci_conf2_read_config_word(unsigned char bus, int devfn, int where, u16 * value)
{
	SET(bus, devfn);
	*value = inw(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return 0;
}

static int pci_conf2_read_config_dword(unsigned char bus, int devfn, int where, u32 * value)
{
	SET(bus, devfn);
	*value = inl(IOADDR(devfn, where));
	outb(0, 0xCF8);
	return 0;
}

static int pci_conf2_write_config_byte(unsigned char bus, int devfn, int where, u8 value)
{
	SET(bus, devfn);
	outb(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
	return 0;
}

static int pci_conf2_write_config_word(unsigned char bus, int devfn, int where, u16 value)
{
	SET(bus, devfn);
	outw(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
	return 0;
}

static int pci_conf2_write_config_dword(unsigned char bus, int devfn, int where, u32 value)
{
	SET(bus, devfn);
	outl(value, IOADDR(devfn, where));
	outb(0, 0xCF8);
	return 0;
}

#undef SET
#undef IOADDR
#undef FUNC

static const struct pci_ops pci_direct_conf2 =
{
	pci_conf2_read_config_byte,
	pci_conf2_read_config_word,
	pci_conf2_read_config_dword,
	pci_conf2_write_config_byte,
	pci_conf2_write_config_word,
	pci_conf2_write_config_dword
};

/*
 * Before we decide to use direct hardware access mechanisms, we try to do some
 * trivial checks to ensure it at least _seems_ to be working -- we just test
 * whether bus 00 contains a host bridge (this is similar to checking
 * techniques used in XFree86, but ours should be more reliable since we
 * attempt to make use of direct access hints provided by the PCI BIOS).
 *
 * This should be close to trivial, but it isn't, because there are buggy
 * chipsets (yes, you guessed it, by Intel and Compaq) that have no class ID.
 */
static int pci_sanity_check(const struct pci_ops *o)
{
	u16 x;
	u8 bus;
	int devfn;
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_DISPLAY_VGA		0x0300
#define PCI_VENDOR_ID_COMPAQ		0x0e11
#define PCI_VENDOR_ID_INTEL		0x8086

	for (bus = 0, devfn = 0; devfn < 0x100; devfn++)
		if ((!o->read_word(bus, devfn, PCI_CLASS_DEVICE, &x) &&
		     (x == PCI_CLASS_BRIDGE_HOST || x == PCI_CLASS_DISPLAY_VGA)) ||
		    (!o->read_word(bus, devfn, PCI_VENDOR_ID, &x) &&
		     (x == PCI_VENDOR_ID_INTEL || x == PCI_VENDOR_ID_COMPAQ)))
			return 1;
	printk_err("PCI: Sanity check failed\n");
	return 0;
}

static const struct pci_ops *pci_check_direct(void)
{
	unsigned int tmp;

	/*
	 * Check if configuration type 1 works.
	 */
	{
		outb(0x01, 0xCFB);
		tmp = inl(0xCF8);
		outl(0x80000000, 0xCF8);
		if (inl(0xCF8) == 0x80000000 &&
		    pci_sanity_check(&pci_direct_conf1)) {
			outl(tmp, 0xCF8);
			printk_debug("PCI: Using configuration type 1\n");
			return &pci_direct_conf1;
		}
		outl(tmp, 0xCF8);
	}

	/*
	 * Check if configuration type 2 works.
	 */
	{
		outb(0x00, 0xCFB);
		outb(0x00, 0xCF8);
		outb(0x00, 0xCFA);
		if (inb(0xCF8) == 0x00 && inb(0xCFA) == 0x00 &&
		    pci_sanity_check(&pci_direct_conf2)) {
			printk_debug("PCI: Using configuration type 2\n");
			return &pci_direct_conf2;
		}
	}

	return 0;
}


int pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val)
{
	int res; 
	res = conf->read_byte(dev->bus->secondary, dev->devfn, where, val);
	printk_spew("Read config byte bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		    dev->bus->secondary, dev->devfn, where, *val, res);
	return res;


}

int pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val)
{
	int res; 
	res = conf->read_word(dev->bus->secondary, dev->devfn, where, val);
	printk_spew( "Read config word bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     dev->bus->secondary, dev->devfn, where, *val, res);
	return res;
}

int pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val)
{
	int res; 
	res = conf->read_dword(dev->bus->secondary, dev->devfn, where, val);
	printk_spew( "Read config dword bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     dev->bus->secondary, dev->devfn, where, *val, res);
	return res;
}

int pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
	printk_spew( "Write config byte bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->devfn, where, val);
	return conf->write_byte(dev->bus->secondary, dev->devfn, where, val);
}

int pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
	printk_spew( "Write config word bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->devfn, where, val);
	return conf->write_word(dev->bus->secondary, dev->devfn, where, val);

}

int pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
	printk_spew( "Write config dword bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->secondary, dev->devfn, where, val);
	return conf->write_dword(dev->bus->secondary, dev->devfn, where, val);
}

int pcibios_read_config_byte(unsigned char bus, unsigned char devfn, u8 where, u8 * val)
{
	int res; 
	res = conf->read_byte(bus, devfn, where, val);
	printk_spew( "Read config byte bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     bus, devfn, where, *val, res);
	return res;
}

int pcibios_read_config_word(unsigned char bus, unsigned char devfn, u8 where, u16 * val)
{
	int res; 
	res = conf->read_word(bus, devfn, where, val);
	printk_spew( "Read config word bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     bus, devfn, where, *val, res);
	return res;

}

int pcibios_read_config_dword(unsigned char bus, unsigned char devfn, u8 where, u32 * val)
{
	int res; 
	res = conf->read_dword(bus, devfn, where, val);
	printk_spew( "Read config dword bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     bus, devfn, where, *val, res);
	return res;

}

int pcibios_write_config_byte(unsigned char bus, unsigned char devfn, u8 where, u8 val)
{
	printk_spew( "Write byte bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     bus, devfn, where, val);
	return conf->write_byte(bus, devfn, where, val);

}

int pcibios_write_config_word(unsigned char bus, unsigned char devfn, u8 where, u16 val)
{
	printk_spew( "Write word bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     bus, devfn, where, val);
	return conf->write_word(bus, devfn, where, val);

}

int pcibios_write_config_dword(unsigned char bus, unsigned char devfn, u8 where, u32 val)
{
    	printk_spew( "Write doubleword bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     bus, devfn, where, val);
	return conf->write_dword(bus, devfn, where, val);
}

/** round a number to an alignment. 
 * @param val the starting value
 * @param roundup Alignment as a power of two
 * @returns rounded up number
 */
unsigned long round(unsigned long val, unsigned long roundup)
{
	/* ROUNDUP MUST BE A POWER OF TWO. */
	unsigned long inverse;
	inverse = ~(roundup - 1);
	val += (roundup - 1);
	val &= inverse;
	return val;
}

unsigned long round_down(unsigned long val, unsigned long round_down)
{
	/* ROUND_DOWN MUST BE A POWER OF TWO. */
	unsigned long inverse;
	inverse = ~(round_down - 1);
	val &= inverse;
	return val;
}

/** Set the method to be used for PCI, type I or type II
 */
void pci_set_method()
{
	conf = &pci_direct_conf1;
	conf = pci_check_direct();
}

/* allocating resources on PCI is a mess. The reason is that 
 * the BAR size is actually two things: one is the size, and
 * the other is the alignment of the data. Take, for example, the 
 * SiS agp hardware. BAR 0 reports a size as follows: 0xf8000008. 
 * This means prefetchable, and you can compute the size of 
 * 0x8000000 (128 Mbytes). But it also turns you that only the 
 * top five bits of the address are decoded. So you can not, for 
 * example, allocate address space at 0x400000 for 0x8000000 bytes, 
 * because in the register that will turn into 0. You have
 * to allocate address space using only the top five bits of the 
 * PCI address space, i.e. you have to start allocating at 0x8000000. 
 * 
 * we have a more complex algorithm for address space allocation in the
 * works, that is actually simple code but gets the desired behavior. 
 * For now, though, we operate as follows: 
 * as you encounter BAR values, just round up the current usage
 * to be aligned to the BAR size. Then allocate. 
 * This has the advantage of being simple, and in practice there are 
 * so few large BAR areas that we expect it to cover all cases. 
 * If we find problems with this strategy we'll go to the more complex
 * algorithm. 
 */
/* it's worse than I thought ... 
 * rules: 
 * bridges contain all sub-bridges, and the address space for mem and 
 * prefetch has to be contiguous. 
 * Anyway, this has gotten so complicated we're going to a one-pass 
 * allocate for now. 
 */


/** Read the resources on all devices of a given pci bus.
 * @param bus bus to read the resources on.
 */
static void read_resources(struct pci_dev *bus)
{
	struct pci_dev *curdev;

	
	/* Walk through all of the devices and find which resources they need. */
	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (curdev->resources > 0) {
			continue;
		}
		curdev->ops->read_resources(curdev);
	}
}

static struct pci_dev *largest_resource(struct pci_dev *bus, struct resource **result_res,
	unsigned long type_mask, unsigned long type)
{
	struct pci_dev *curdev;
	struct pci_dev *result_dev = 0;
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

#if PCI_ALLOCATE_TIGHT
void compute_allocate_resource(
	struct pci_dev *bus,
	struct resource *bridge,
	unsigned long type_mask,
	unsigned long type)
{
	struct pci_dev *dev;
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
		min_align = log2(IO_ALIGN);
	}
	if (bridge->flags & IORESOURCE_MEM) {
		min_align = log2(MEM_ALIGN);
	}

	printk_spew("PCI: %02x:%02x.%01x compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d\n", 
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
	while(dev = largest_resource(bus, &resource, type_mask, type)) {
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
				"PCI: %02x:%02x.%01x %02x *  [0x%08lx - 0x%08lx] %s\n",
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

	printk_spew("PCI: %02x:%02x.%01x compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d done\n", 
		bus->bus->secondary,
		PCI_SLOT(bus->devfn), PCI_FUNC(bus->devfn),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);


}
#else
/* As close to the original algorithm as possible, no optimization */
void compute_allocate_resource(
	struct pci_dev *bus,
	struct resource *bridge,
	unsigned long type_mask,
	unsigned long type)
{
	struct pci_dev *dev;
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
		min_align = log2(IO_ALIGN);
	}
	if (bridge->flags & IORESOURCE_MEM) {
		min_align = log2(MEM_ALIGN);
	}

	printk_spew("PCI: %02x:%02x.%01x compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d\n", 
		bus->bus->secondary,
		PCI_SLOT(bus->devfn), PCI_FUNC(bus->devfn),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);

	/* Make certain I have read in all of the resources */
	read_resources(bus);

	for(dev = bus->children; dev; dev = dev->sibling) {
		int i;
		for(i = 0; i < dev->resources; i++) {
			resource = &dev->resource[i];
			/* If it isn't a bridge resource ignore it */
			if (!(resource->flags & IORESOURCE_PCI_BRIDGE)) {
				continue;
			}
			/* If it isn't the right kind of resource ignore it */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			resource->base = base;
			compute_allocate_resource(dev, resource, type_mask, type);
			resource->flags |= IORESOURCE_SET;
			base = round(resource->base + resource->size -1, 
				resource->gran);
		}
	}
	for(dev = bus->children; dev; dev = dev->sibling) {
		int i;
		for(i = 0; i < dev->resources; i++) {
			unsigned long size;
			resource = &dev->resource[i];
			/* If it isn't the right kind of resource ignore it */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			if (!resource->size) {
				continue;
			}
			if (resource->flags & IORESOURCE_PCI_BRIDGE) {
				continue;
			}

			/* Propogate the resource alignment to the bridge register  */
			if (resource->align > bridge->align) {
				bridge->align = resource->align;
			}

			/* Make certain that iosize is a minimum size */
			size = round(resource->size, 1UL << min_align);

			/* base must be aligned to the io size */
			base = round(base, size);
			
			if ((base + size -1) <= resource->limit) {
				resource->base = base;
				resource->flags |= IORESOURCE_SET;
				base += size;
			
				printk_spew(
					"PCI: %02x:%02x.%01x %02x *  [0x%08lx - 0x%08lx] %s\n",
					dev->bus->secondary, 
					PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn), 
					resource->index, 
					resource->base, resource->base + resource->size -1,
					(resource->flags & IORESOURCE_IO)? "io":
					(resource->flags & IORESOURCE_PREFETCH)? "prefmem": "mem");
			}
		}
	}
	/* A pci bridge resource does not need to be a power
	 * of two size, but it does have a minimum granularity.
	 * Round the size up to that minimum granularity so we
	 * know not to place something else at an address postitively
	 * decoded by the bridge.
	 */
	bridge->size = round(base, 1UL << bridge->gran) - bridge->base;

	printk_spew("PCI: %02x:%02x.%01x compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d done\n", 
		bus->bus->secondary,
		PCI_SLOT(bus->devfn), PCI_FUNC(bus->devfn),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);
}

#endif

static void allocate_vga_resource(void)
{
	/* FIXME handle the VGA pallette snooping */
	struct pci_dev *dev, *vga, *bus;
	bus = vga = 0;
	for(dev = pci_devices; dev; dev = dev->next) {
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
void assign_resources(struct pci_dev *bus)
{
	struct pci_dev *curdev;

	printk_debug("ASSIGN RESOURCES, bus %d\n", bus->secondary);

	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		curdev->ops->set_resources(curdev);
	}
}

static void enable_resources(struct pci_dev *bus)
{
	struct pci_dev *curdev;

	/* Walk through the chain of all pci devices and enable them.
	 * This is effectively a breadth first traversal so we should
	 * not have enalbing ordering problems.
	 */
	for (curdev = pci_devices; curdev; curdev = curdev->next) {
		u16 command;
		pci_read_config_word(curdev, PCI_COMMAND, &command);
		command |= curdev->command;
		printk_debug("PCI: %02x:%02x.%01x cmd <- %02x\n",
			curdev->bus->secondary,
			PCI_SLOT(curdev->devfn), PCI_FUNC(curdev->devfn),
			command);
		pci_write_config_word(curdev, PCI_COMMAND, command);
	}
}

/** Enumerate the resources on the PCI by calling pci_init
 */
void pci_enumerate(void)
{
	printk_info("Scanning PCI bus...");
	// scan it. 
	pci_init();
	printk_info("done\n");
}

/** Starting at the root, compute what resources are needed and allocate them. 
 * We start memory, prefetchable memory at PCI_MEM_START. I/O starts at 
 * PCI_IO_START. Since the assignment is hierarchical we set the values
 * into the pci_root struct. 
 */
void pci_configure(void)
{
	struct pci_dev *root = &pci_root;
	printk_info("Allocating PCI resources...");
	printk_debug("\n");


	root->ops->read_resources(root);

	/* Make certain the io devices are allocated somewhere
	 * safe.
	 */
	root->resource[0].base = PCI_IO_START;
	root->resource[0].flags |= IORESOURCE_SET;
	/* Now reallocate the pci resources memory with the
	 * highest addresses I can manage.
	 */
#if PCI_ALLOCATE_TIGHT
	root->resource[1].base = 
		round_down(PCI_MEM_HIGH - root->resource[1].size,
			1UL << root->resource[1].align);
#else
	root->resource[1].base = PCI_MEM_START;
#endif
	root->resource[1].flags |= IORESOURCE_SET;
	// now just set things into registers ... we hope ...
	root->ops->set_resources(root);

	allocate_vga_resource();

	printk_info("done.\n");
}

/** Starting at the root, walk the tree and enable all devices/bridges. 
 * What really happens is computed COMMAND bits get set in register 4
 */
void pci_enable(void)
{
	printk_info("Enabling PCI resourcess...");

	/* now enable everything. */
	enable_resources(&pci_root);
	printk_info("done.\n");
}

/** Starting at the root, walk the tree and call a driver to
 *  do device specific setup.
 */
void pci_initialize(void)
{
	struct pci_dev *dev;

	printk_info("Initializing PCI devices...\n");
	for (dev = pci_devices; dev; dev = dev->next) {
		if (dev->ops->init) {
			printk_debug("PCI: %02x:%02x.%01x init\n",
				dev->bus->secondary,
				PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
			dev->ops->init(dev);
		}
	}
	printk_info("PCI devices initialized\n");
}

void
handle_superio(int pass, struct superio *all_superio[], int nsuperio)
{
	int i;
	struct superio *s;

	printk_debug("handle_superio start, s %p nsuperio %d s->super %p\n",
		     s, nsuperio, s->super);

	for (i = 0; i < nsuperio; i++){
		s = all_superio[i];
		printk_debug(__FUNCTION__
			     " Pass %d, check #%d, s %p s->super %p\n",
			     pass, i, s, s->super);
		if (!s->super) {
			printk_debug(__FUNCTION__
				     " Pass %d, Skipping #%d as it has no superio pointer!\n",
				     pass, i);
			continue;
		}

		printk_debug("handle_superio: Pass %d, Superio %s\n",
			     pass, s->super->name);

		// if no port is assigned use the defaultport
		printk_info(__FUNCTION__ "  port 0x%x, defaultport 0x%x\n",
			    s->port, s->super->defaultport);
		if (!s->port)
			s->port = s->super->defaultport;

		printk_info(__FUNCTION__ "  Using port 0x%x\n", s->port);

		// need to have both pre_pci_init and devfn defined.
		if (s->super->pre_pci_init && (pass == 0)) {
			printk_debug("  Call pre_pci_init\n");
			s->super->pre_pci_init(s);
		} else if (s->super->init && (pass == 1)) {
			printk_debug("  Call init\n");
			s->super->init(s);
		} else if (s->super->finishup && (pass == 2)) {
			printk_debug("  Call finishup\n");
			s->super->finishup(s);
		}
		printk_debug(__FUNCTION__ " Pass %d, done #%d\n", pass, i);
	}
	printk_debug("handle_superio done\n");
}

void
handle_southbridge(int pass, struct southbridge *s, int nsouthbridge)
{
	int i;
	for (i = 0; i < nsouthbridge; i++, s++) {

		if (!s->southbridge)
			continue;
		printk_debug("handle_southbridge: Pass %d, Superio %s\n", pass, 
			     s->southbridge->name);

		// need to have both pre_pci_init and devfn defined.
		if (s->southbridge->pre_pci_init && (pass == 0) && (s->devfn)) {
			printk_debug("  Call pre_pci_init\n");
			s->southbridge->pre_pci_init(s);
		} else {
			// first, have to set up any device not set up. 
			// policy: we ignore the devfn here. First, it's in the pcidev, and
			// second, it's really only to be used BEFORE pci config is done. 
			if (!s->device)
				s->device = pci_find_device(s->southbridge->vendor, 
							    s->southbridge->device, 0);

			if (!s->device) {
				// not there!
				printk_info("  No such device\n");
				continue;
			}
			// problem. We have to handle multiple devices of same type. 
			// We don't do this yet. One way is to mark the pci device used at
			// this point, i.e. 
			// s->device->inuse = 1
			// and then continue looking if the device is in use.
			// For now, let's get this basic thing to work.
			if (s->southbridge->init && (pass == 1)) {
				printk_debug("  Call init\n");
				s->southbridge->init(s);
			} else if (s->southbridge->finishup && (pass == 2)) {
				printk_debug("  Call finishup\n");
				s->southbridge->finishup(s);
			}
		}
	}
}

