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

#include <pci.h>
#undef __KERNEL__
#include <arch/io.h>
#include <printk.h>

#define ONEMEG (1 << 20)

#define PCI_MEM_START 0x80000000
#define PCI_IO_START 0x1000

// historical functions, sometimes very useful. 
/*
 *    Write the special configuration registers on the INTEL
 */
void intel_conf_writeb(unsigned long port, unsigned char value)
{
	unsigned char whichbyte = port & 3;
	port &= (~3);
	outl(port, PCI_CONF_REG_INDEX);
	outb(value, PCI_CONF_REG_DATA + whichbyte);
}

/*
 *    Read the special configuration registers on the INTEL
 */
unsigned char intel_conf_readb(unsigned long port)
{
	unsigned char whichbyte = port & 3;
	port &= (~3);
	outl(port, PCI_CONF_REG_INDEX);
	return inb(PCI_CONF_REG_DATA + whichbyte);
}


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
	res = conf->read_byte(dev->bus->number, dev->devfn, where, val);
	printk_spew("Read config byte bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		    dev->bus->number, dev->devfn, where, *val, res);
	return res;


}

int pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val)
{
	int res; 
	res = conf->read_word(dev->bus->number, dev->devfn, where, val);
	printk_spew( "Read config word bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     dev->bus->number, dev->devfn, where, *val, res);
	return res;
}

int pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val)
{
	int res; 
	res = conf->read_dword(dev->bus->number, dev->devfn, where, val);
	printk_spew( "Read config dword bus %d,devfn 0x%x,reg 0x%x,val 0x%x,res 0x%x\n",
		     dev->bus->number, dev->devfn, where, *val, res);
	return res;
}

int pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
	printk_spew( "Write config byte bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->number, dev->devfn, where, val);
	return conf->write_byte(dev->bus->number, dev->devfn, where, val);
}

int pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
	printk_spew( "Write config word bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->number, dev->devfn, where, val);
	return conf->write_word(dev->bus->number, dev->devfn, where, val);

}

int pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
	printk_spew( "Write config dword bus %d, devfn 0x%x, reg 0x%x, val 0x%x\n",
		     dev->bus->number, dev->devfn, where, val);
	return conf->write_dword(dev->bus->number, dev->devfn, where, val);
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
	// ROUNDUP MUST BE A POWER OF TWO. 
	unsigned long inverse;
	inverse = ~(roundup - 1);
	val += (roundup - 1);
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


/** Given a desired amount of io, round it to IO_BRIDGE_ALIGN
 * @param amount Amount of memory desired. 
 */
unsigned long iolimit(unsigned long amount)
{
	amount = round(amount, IO_BRIDGE_ALIGN) - 1;
	return amount;
}

/** Given a desired amount of memory, round it to ONEMEG
 * @param amount Amount of memory desired. 
 */
unsigned long memlimit(unsigned long amount)
{
	amount = round(amount, ONEMEG) - 1;
	return amount;
}

/** Compute and allocate the io for this bus. 
 * @param bus Pointer to the struct for this bus. 
 */
void compute_allocate_io(struct pci_bus *bus)
{
	int i;
	struct pci_bus *curbus;
	struct pci_dev *curdev;
	unsigned long io_base;

	io_base = bus->iobase;
	printk_debug("compute_allocate_io: base 0x%lx\n", bus->iobase);

	/* First, walk all the bridges. When you return, grow the limit of the current bus
	   since sub-busses need IO rounded to 4096 */
	for (curbus = bus->children; curbus; curbus = curbus->next) {
		curbus->iobase = io_base;
		compute_allocate_io(curbus);
		io_base = round(curbus->iolimit, IO_BRIDGE_ALIGN);
		printk_debug("BUSIO: done Bridge Bus 0x%x, iobase now 0x%lx\n",
			     curbus->number, io_base);
	}

	/* Walk through all the devices on current bus and compute IO address space.*/
	for (curdev = bus->devices; curdev; curdev = curdev->sibling) {
		u32 class_revision;
		/* FIXME Special case for VGA for now just note
		 * we have an I/O resource later make certain
		 * we don't have a device conflict.
		 */
		pci_read_config_dword(curdev, PCI_CLASS_REVISION, &class_revision);
		if (((class_revision >> 24) == 0x03) && 
		    ((class_revision >> 16) != 0x380)) {
			printk_debug("Running VGA fix...\n");
			/* All legacy VGA cards have I/O space registers */
			curdev->command |= PCI_COMMAND_IO;	
		}
		for (i = 0; i < 6; i++) {
			unsigned long size = curdev->size[i];
			if (size & PCI_BASE_ADDRESS_SPACE_IO) {
				unsigned long iosize = size & PCI_BASE_ADDRESS_IO_MASK;
				if (!iosize)
					continue;

				printk_debug("DEVIO: Bus 0x%x, devfn 0x%x, reg 0x%x: "
					     "iosize 0x%lx\n",
					     curdev->bus->number, curdev->devfn, i, iosize);
				// Make sure that iosize is a minimum 
				// size. 
				iosize = round(iosize, IO_ALIGN);
				// io_base must be aligned to the io size.
				io_base = round(io_base, iosize);
				printk_debug("  rounded size %d base 0x%x\n",
					     iosize, io_base);
				curdev->base_address[i] = io_base;
				// some chipsets allow us to set/clear the IO bit. 
				// (e.g. VIA 82c686a.) So set it to be safe)
				curdev->base_address[i] |= 
					PCI_BASE_ADDRESS_SPACE_IO;
				printk_debug("-->set base to 0x%lx\n", io_base);
				io_base += iosize;
				curdev->command |= PCI_COMMAND_IO;
			}
		}
	}
	bus->iolimit = iolimit(io_base);

	printk_debug("BUS %d: set iolimit to 0x%lx\n", bus->number, bus->iolimit);
}

/** Compute and allocate the memory for this bus. 
 * @param bus Pointer to the struct for this bus. 
 */
void compute_allocate_mem(struct pci_bus *bus)
{
	int i;
	struct pci_bus *curbus;
	struct pci_dev *curdev;
	unsigned long mem_base;

	mem_base = bus->membase;
	printk_debug("compute_allocate_mem: base 0x%lx\n", bus->membase);

	/* First, walk all the bridges. When you return, grow the limit of the current bus
	   since sub-busses need MEMORY rounded to 1 Mega */
	for (curbus = bus->children; curbus; curbus = curbus->next) {
		curbus->membase = mem_base;
		compute_allocate_mem(curbus);
		mem_base = round(curbus->memlimit, ONEMEG);
		printk_debug("BUSMEM: Bridge Bus 0x%x,membase now 0x%lx\n",
			     curbus->number, mem_base);
	}

	/* Walk through all the devices on current bus and oompute MEMORY address space.*/
	for (curdev = bus->devices; curdev; curdev = curdev->sibling) {
		for (i = 0; i < 6; i++) {
			unsigned long size = curdev->size[i];
			unsigned long memorysize = size & (PCI_BASE_ADDRESS_MEM_MASK);
			unsigned long type = size & (~PCI_BASE_ADDRESS_MEM_MASK);
			if (!memorysize) {
				continue;
			}
			
			if (type & PCI_BASE_ADDRESS_SPACE_IO) {
				continue;
			}

			// we don't support the 1M type
			if (type & PCI_BASE_ADDRESS_MEM_TYPE_1M) {
				continue;
			}

			// if it's prefetch type, continue;
			if (type & PCI_BASE_ADDRESS_MEM_PREFETCH) {
				continue;
			}

			// now mask out all but the 32 or 64 bits
			type &= PCI_BASE_ADDRESS_MEM_TYPE_MASK;

			// I'm pretty sure this test is not needed, but ...
			if ((type == PCI_BASE_ADDRESS_MEM_TYPE_32) ||
			    (type == PCI_BASE_ADDRESS_MEM_TYPE_64)) {
				/* this is normal memory space */
				unsigned long regmem;

				/* PCI BUS Spec suggests that the memory address should be
				   consumed in 4KB unit */
				regmem = round(memorysize, MEM_ALIGN);
				mem_base = round(mem_base, regmem);
				printk_debug("DEVMEM: Bus 0x%x, devfn 0x%x, reg 0x%x: "
					     "memsize 0x%lx\n",
					     curdev->bus->number, curdev->devfn, i, regmem);
				curdev->base_address[i] = mem_base;
				printk_debug("-->set base to 0x%lx\n", mem_base);
				mem_base += regmem;
				curdev->command |= PCI_COMMAND_MEMORY;
				// for 64-bit BARs, the odd ones don't count
				if (type == PCI_BASE_ADDRESS_MEM_TYPE_64)
					continue;

			}
		}
	}
	bus->memlimit = memlimit(mem_base);

	printk_debug("BUS %d: set memlimit to 0x%lx\n", bus->number, bus->memlimit);
}

/** Compute and allocate the prefetch memory for this bus. 
 * @param bus Pointer to the struct for this bus. 
 */
void compute_allocate_prefmem(struct pci_bus *bus)
{
	int i;
	struct pci_bus *curbus;
	struct pci_dev *curdev;
	unsigned long prefmem_base;

	prefmem_base = bus->prefmembase;
	printk_debug("Compute_allocate_prefmem: base 0x%lx\n", bus->prefmembase);

	/* First, walk all the bridges. When you return, grow the limit of the current bus
	   since sub-busses need MEMORY rounded to 1 Mega */
	for (curbus = bus->children; curbus; curbus = curbus->next) {
		curbus->prefmembase = prefmem_base;
		compute_allocate_prefmem(curbus);
		prefmem_base = round(curbus->prefmemlimit, ONEMEG);
		printk_debug("BUSPREFMEM: Bridge Bus 0x%x, prefmem base now 0x%lx\n",
			     curbus->number, prefmem_base);
	}

	/* Walk through all the devices on current bus and oompute PREFETCHABLE MEMORY address space.*/
	for (curdev = bus->devices; curdev; curdev = curdev->sibling) {
		for (i = 0; i < 6; i++) {
			unsigned long size = curdev->size[i];
			unsigned long memorysize = size & (PCI_BASE_ADDRESS_MEM_MASK);
			unsigned long type = size & (~PCI_BASE_ADDRESS_MEM_MASK);

			if (!memorysize)
				continue;

			if (type & PCI_BASE_ADDRESS_SPACE_IO) {
				continue;
			}

			// we don't support the 1M type
			if (type & PCI_BASE_ADDRESS_MEM_TYPE_1M) {
				printk_warning(__FUNCTION__ ": 1M memory not supported\n");
				continue;
			}
		      
			// if it's not a prefetch type, continue;
			if (! (type & PCI_BASE_ADDRESS_MEM_PREFETCH))
				continue;
			// this should be a function some day ... comon code with 
			// the non-prefetch allocate
			// now mask out all but the 32 or 64 bit type info
			type &= PCI_BASE_ADDRESS_MEM_TYPE_MASK;
			// if all these names confuse you, they confuse me too!
			if ((type == PCI_BASE_ADDRESS_MEM_TYPE_32) ||
			    (type == PCI_BASE_ADDRESS_MEM_TYPE_64)) {
				unsigned long regmem;

				/* PCI BUS Spec suggests that the memory address should be
				   consumed in 4KB unit */
				regmem = round(memorysize, MEM_ALIGN);
				prefmem_base = round(prefmem_base, regmem);
				printk_debug("DEVPREFMEM: Bus 0x%x, devfn 0x%x, reg 0x%x: "
					     "prefmemsize 0x%lx\n",
					     curdev->bus->number, curdev->devfn, i, regmem);
				curdev->base_address[i] = prefmem_base;
				printk_debug("-->set base to 0x%lx\n", prefmem_base);
				prefmem_base += regmem;
				curdev->command |= PCI_COMMAND_MEMORY;
				// for 64-bit BARs, the odd ones don't count
				if (type == PCI_BASE_ADDRESS_MEM_TYPE_64)
					continue;
			}
		}
	}
	bus->prefmemlimit = memlimit(prefmem_base);

	printk_debug("BUS %d: set prefmemlimit to 0x%lx\n", bus->number, bus->prefmemlimit);
}

/** Compute and allocate resources. 
 * This is a one-pass process. We first compute all the IO, then 
 * memory, then prefetchable memory. 
 * This is really only called at the top level
 * @param bus Pointer to the struct for this bus. 
 */
void compute_allocate_resources(struct pci_bus *bus)
{
	printk_debug("COMPUTE_ALLOCATE: do IO\n");
	compute_allocate_io(bus);

	printk_debug("COMPUTE_ALLOCATE: do MEM\n");
	compute_allocate_mem(bus);

	// now put the prefetchable memory at the end of the memory
	bus->prefmembase = round(bus->memlimit, ONEMEG);

	printk_debug("COMPUTE_ALLOCATE: do PREFMEM\n");
	compute_allocate_prefmem(bus);
}

/** Assign the computed resources to the bridges and devices on the bus.
 * Recurse to any bridges found on this bus first. Then do the devices
 * on this bus. 
 * @param bus Pointer to the structure for this bus
 */ 
void assign_resources(struct pci_bus *bus)
{
	struct pci_dev *curdev = pci_devices;
	struct pci_bus *curbus;

	printk_debug("ASSIGN RESOURCES, bus %d\n", bus->number);

	/* walk trhough all the buses, assign resources for bridges */
	for (curbus = bus->children; curbus; curbus = curbus->next) {
		curbus->self->command = 0;

		/* set the IO ranges
		   WARNING: we don't really do 32-bit addressing for IO yet! */
		if (curbus->iobase) {
			curbus->self->command |= PCI_COMMAND_IO;
			pci_write_config_byte(curbus->self, PCI_IO_BASE,
					      curbus->iobase >> 8);
			pci_write_config_byte(curbus->self, PCI_IO_LIMIT,
					      curbus->iolimit >> 8);
			printk_debug("Bus 0x%x Child Bus %x iobase to 0x%x iolimit 0x%x\n",
				     bus->number,curbus->number, curbus->iobase, curbus->iolimit);
		}

		// set the memory range
		if (curbus->membase) {
			curbus->self->command |= PCI_COMMAND_MEMORY;
			pci_write_config_word(curbus->self, PCI_MEMORY_BASE,
					      curbus->membase >> 16);
			pci_write_config_word(curbus->self, PCI_MEMORY_LIMIT,
					      curbus->memlimit >> 16);
			printk_debug("Bus 0x%x Child Bus %x membase to 0x%x memlimit 0x%x\n",
				     bus->number,curbus->number, curbus->membase, curbus->memlimit);

		}

		// set the prefetchable memory range
		if (curbus->prefmembase) {
			curbus->self->command |= PCI_COMMAND_MEMORY;
			pci_write_config_word(curbus->self, PCI_PREF_MEMORY_BASE,
					      curbus->prefmembase >> 16);
			pci_write_config_word(curbus->self, PCI_PREF_MEMORY_LIMIT,
					      curbus->prefmemlimit >> 16);
			printk_debug("Bus 0x%x Child Bus %x prefmembase to 0x%x prefmemlimit 0x%x\n",
				     bus->number,curbus->number, curbus->prefmembase,
				     curbus->prefmemlimit);

		}
		curbus->self->command |= PCI_COMMAND_MASTER;
		assign_resources(curbus);
	}

	for (curdev = bus->devices; curdev; curdev = curdev->sibling) {
		int i;
		for (i = 0; i < 6; i++) {
			unsigned long reg;
			if (curdev->base_address[i] == 0)
				continue;

			reg = PCI_BASE_ADDRESS_0 + (i << 2);
			pci_write_config_dword(curdev, reg, curdev->base_address[i]);
			printk_debug("Bus 0x%x devfn 0x%x reg 0x%x base to 0x%lx\n",
				     curdev->bus->number, curdev->devfn, i, 
				     curdev->base_address[i]);
		}
		/* set a default latency timer */
		pci_write_config_byte(curdev, PCI_LATENCY_TIMER, 0x40);
	}
}

void enable_resources(struct pci_bus *bus)
{
	struct pci_dev *curdev = pci_devices;

	/* walk through the chain of all pci device, this time we don't have to deal
	   with the device v.s. bridge stuff, since every bridge has its own pci_dev
	   assocaited with it */
	for (curdev = pci_devices; curdev; curdev = curdev->next) {
		u16 command;
		pci_read_config_word(curdev, PCI_COMMAND, &command);
		command |= curdev->command;
		printk_debug("DEV Set command bus 0x%02x devfn 0x%02x to 0x%02x\n",
			     curdev->bus->number, curdev->devfn, command);
		pci_write_config_word(curdev, PCI_COMMAND, command);
	}
}

/** Enumerate the resources on the PCI by calling pci_init
 */
void pci_enumerate()
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
void pci_configure()
{
	printk_info("Allocating PCI resources...");

	pci_root.membase = PCI_MEM_START;
	pci_root.prefmembase = PCI_MEM_START;
	pci_root.iobase = PCI_IO_START;

	compute_allocate_resources(&pci_root);

	// now just set things into registers ... we hope ...
	assign_resources(&pci_root);

	printk_info("done.\n");
}

/** Starting at the root, walk the tree and enable all devices/bridges. 
 * What really happens is computed COMMAND bits get set in register 4
 */
void pci_enable()
{
	printk_info("Enabling PCI resourcess...");

	// now enable everything.
	enable_resources(&pci_root);
	printk_info("done.\n");
}

void pci_zero_irq_settings(void)
{
	struct pci_dev *pcidev;
	unsigned char line;
  
	printk_info("Zeroing PCI IRQ settings...");

	pcidev = pci_devices;
  
	while (pcidev) {
		pci_read_config_byte(pcidev, 0x3d, &line);
		if (line) {
			pci_write_config_byte(pcidev, 0x3c, 0);
		}
		pcidev = pcidev->next;
	}
	printk_info("done.\n");
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








