#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <mem.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <device/chip.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"
#include "amdk8.h"

struct mem_range *sizeram(void)
{
	unsigned long mmio_basek;
	static struct mem_range mem[10];
	device_t dev;
	int i, idx;

#warning "FIXME handle interleaved nodes"
	dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
	if (!dev) {
		printk_err("Cannot find PCI: 0:18.1\n");
		return 0;
	}
	mmio_basek = (dev_root.resource[1].base >> 10);
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

#if 1
#warning "FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M MMIO hole"
	/* Round the mmio hold to 256M */
	mmio_basek &= ~((256*1024) - 1);
#endif

#if 0
	printk_debug("mmio_base: %dKB\n", mmio_basek);
#endif

	for (idx = i = 0; i < 8; i++) {
		uint32_t base, limit;
		unsigned basek, limitk, sizek;
		base  = pci_read_config32(dev, 0x40 + (i<<3));
		limit = pci_read_config32(dev, 0x44 + (i<<3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		basek = (base & 0xffff0000) >> 2;
		limitk = ((limit + 0x00010000) & 0xffff0000) >> 2;
		sizek = limitk - basek;
		if ((idx > 0) &&
		    ((mem[idx - 1].basek + mem[idx - 1].sizek) == basek)) {
			mem[idx -1].sizek += sizek;
		} else {
			mem[idx].basek = basek;
			mem[idx].sizek = sizek;
			idx++;
		}
	
		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((mem[idx-1].basek < ((8*64)+(8*16))) /* 640 */ && 
		    (mem[idx-1].sizek > ((8*64)+(16*16))) /* 768 */ ) {
#warning "FIXME: this left 0xA0000 to 0xBFFFF undefined"
			mem[idx].basek = (8*64)+(16*16);
			mem[idx].sizek = mem[idx-1].sizek - ((8*64)+(16*16));
			mem[idx-1].sizek = ((8*64)+(8*16)) - mem[idx-1].basek;
			idx++;
		}	
		
		/* See if I need to split the region to accomodate pci memory space */
		if ((mem[idx - 1].basek <= mmio_basek) &&
		    ((mem[idx - 1].basek + mem[idx - 1].sizek) >  mmio_basek)) {
			if (mem[idx - 1].basek < mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - mem[idx - 1].basek;
				mem[idx].basek = mmio_basek;
				mem[idx].sizek = mem[idx - 1].sizek - pre_sizek;
				mem[idx - 1].sizek = pre_sizek;
				idx++;
			}
			if ((mem[idx - 1].basek + mem[idx - 1].sizek) <= 4*1024*1024) {
				idx -= 1;
			}
			else {
				mem[idx - 1].basek = 4*1024*1024;
				mem[idx - 1].sizek -= (4*1024*1024 - mmio_basek);
			}
		}
	}
#if 1
	for (i = 0; i < idx; i++) {
		printk_debug("mem[%d].basek = %08x mem[%d].sizek = %08x\n",
			     i, mem[i].basek, i, mem[i].sizek);
	}
#endif
	while(idx < sizeof(mem)/sizeof(mem[0])) {
		mem[idx].basek = 0;
		mem[idx].sizek = 0;
		idx++;
	}
	return mem;
}

#define F1_DEVS 8
static device_t __f1_dev[F1_DEVS];

#if 0
static void debug_f1_devs(void)
{
	int i;
	for(i = 0; i < F1_DEVS; i++) {
		device_t dev;
		dev = __f1_dev[i];
		if (dev) {
			printk_debug("__f1_dev[%d]: %s bus: %p\n",
				i, dev_path(dev), dev->bus);
		}
	}
}
#endif

static void get_f1_devs(void)
{
	int i;
	if (__f1_dev[0]) {
		return;
	}
	for(i = 0; i < F1_DEVS; i++) {
		__f1_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 1));
	}
	if (!__f1_dev[0]) {
		die("Cannot find 0:0x18.1\n");
	}
}

static uint32_t f1_read_config32(unsigned reg)
{
	get_f1_devs();
	return pci_read_config32(__f1_dev[0], reg);
}

static void f1_write_config32(unsigned reg, uint32_t value)
{
	int i;
	get_f1_devs();
	for(i = 0; i < F1_DEVS; i++) {
		device_t dev;
		dev = __f1_dev[i];
		if (dev) {
			pci_write_config32(dev, reg, value);
		}
	}
}

static unsigned int amdk8_nodeid(device_t dev)
{
	return (dev->path.u.pci.devfn >> 3) - 0x18;
}


static unsigned int amdk8_scan_chains(device_t dev, unsigned int max)
{
	unsigned nodeid;
	unsigned link;
	nodeid = amdk8_nodeid(dev);
#if 1
	printk_debug("amdk8_scan_chains max: %d starting...\n", max);
#endif
	for(link = 0; link < dev->links; link++) {
		uint32_t link_type;
		uint32_t busses, config_busses;
		unsigned free_reg, config_reg;
		dev->link[link].cap = 0x80 + (link *0x20);
		do {
			link_type = pci_read_config32(dev, dev->link[link].cap + 0x18);
		} while(link_type & ConnectionPending);
		if (!(link_type & LinkConnected)) {
			continue;
		}
		do {
			link_type = pci_read_config32(dev, dev->link[link].cap + 0x18);
		} while(!(link_type & InitComplete));
		if (!(link_type & NonCoherent)) {
			continue;
		}
		/* See if there is an available configuration space mapping register in function 1. */
		free_reg = 0;
		for(config_reg = 0xe0; config_reg <= 0xec; config_reg += 4) {
			uint32_t config;
			config = f1_read_config32(config_reg);
			if (!free_reg && ((config & 3) == 0)) {
				free_reg = config_reg;
				continue;
			}
			if (((config & 3) == 3) && 
				(((config >> 4) & 7) == nodeid) &&
				(((config >> 8) & 3) == link)) {
				break;
			}
		}
		if (free_reg && (config_reg > 0xec)) {
			config_reg = free_reg;
		}
		/* If we can't find an available configuration space mapping register skip this bus */
		if (config_reg > 0xec) {
			continue;
		}

		/* Set up the primary, secondary and subordinate bus numbers.  We have
		 * no idea how many busses are behind this bridge yet, so we set the subordinate
		 * bus number to 0xff for the moment.
		 */
		dev->link[link].secondary = ++max;
		dev->link[link].subordinate = 0xff;

		/* Read the existing primary/secondary/subordinate bus
		 * number configuration.
		 */
		busses = pci_read_config32(dev, dev->link[link].cap + 0x14);
		config_busses = f1_read_config32(config_reg);
		
		/* Configure the bus numbers for this bridge: the configuration
		 * transactions will not be propagates by the bridge if it is not
		 * correctly configured
		 */
		busses &= 0xff000000;
		busses |= (((unsigned int)(dev->bus->secondary) << 0) |
			((unsigned int)(dev->link[link].secondary) << 8) |
			((unsigned int)(dev->link[link].subordinate) << 16));
		pci_write_config32(dev, dev->link[link].cap + 0x14, busses);

		config_busses &= 0x000fc88;
		config_busses |= 
			(3 << 0) |  /* rw enable, no device compare */
			(( nodeid & 7) << 4) | 
			(( link & 3 ) << 8) |  
			((dev->link[link].secondary) << 16) |
			((dev->link[link].subordinate) << 24);
		f1_write_config32(config_reg, config_busses);

#if 1
		printk_debug("Hyper transport scan link: %d max: %d\n", link, max);
#endif		
		/* Now we can scan all of the subordinate busses i.e. the chain on the hypertranport link */
		max = hypertransport_scan_chain(&dev->link[link], max);

#if 1
		printk_debug("Hyper transport scan link: %d new max: %d\n", link, max);
#endif		

		/* We know the number of busses behind this bridge.  Set the subordinate
		 * bus number to it's real value
		 */
		dev->link[link].subordinate = max;
		busses = (busses & 0xff00ffff) |
			((unsigned int) (dev->link[link].subordinate) << 16);
		pci_write_config32(dev, dev->link[link].cap + 0x14, busses);

		config_busses = (config_busses & 0x00ffffff) | (dev->link[link].subordinate << 24);
		f1_write_config32(config_reg, config_busses);
#if 1
		printk_debug("Hypertransport scan link done\n");
#endif		
	}
#if 1
	printk_debug("amdk8_scan_chains max: %d done\n", max);
#endif
	return max;
}


static unsigned amdk8_find_iopair(unsigned nodeid, unsigned link)
{
	unsigned free_reg, reg;

	free_reg = 0;
	for(reg = 0xc0; reg <= 0xd8; reg += 0x8) {
		uint32_t base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		/* Do I have a free register */
		if (!free_reg && ((base & 3) == 0)) {
			free_reg = reg;
		}
		/* Do I have a match for this node and link? */
		if (((base & 3) == 3) &&
			((limit & 7) == nodeid) &&
			(((limit >> 4) & 3) == link)) {
			break;
		}
	}
	/* If I didn't find an exact match return a free register */
	if (reg > 0xd8) {
		reg = free_reg;
	}
	/* Return an available I/O pair or 0 on failure */
	return reg;
}

static unsigned amdk8_find_mempair(unsigned nodeid, unsigned link)
{
	unsigned free_reg, reg;
	free_reg = 0;
	for(reg = 0x80; reg <= 0xb8; reg += 0x8) {
		uint32_t base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		/* Do I have a free register */
		if (!free_reg && ((base & 3) == 0)) {
			free_reg = reg;
		}
		/* Do I have a match for this node and link? */
		if (((base & 3) == 3) &&
			((limit & 7) == nodeid) &&
			(((limit >> 4) & 3) == link)) {
			break;
		}
	}
	/* If I didn't find an exact match return a free register */
	if (reg > 0xb8) {
		reg = free_reg;
	}
	/* Return an available I/O pair or 0 on failure */
	return reg;
}

static void amdk8_link_read_bases(device_t dev, unsigned nodeid, unsigned link)
{
	unsigned int reg = dev->resources;
	unsigned index;
	
	/* Initialize the io space constraints on the current bus */
	index = amdk8_find_iopair(nodeid, link);
	if (index) {
		dev->resource[reg].base  = 0;
		dev->resource[reg].size  = 0;
		dev->resource[reg].align = log2(HT_IO_HOST_ALIGN);
		dev->resource[reg].gran  = log2(HT_IO_HOST_ALIGN);
		dev->resource[reg].limit = 0xffffUL;
		dev->resource[reg].flags = IORESOURCE_IO;
		dev->resource[reg].index = index | (link & 0x3);
		compute_allocate_resource(&dev->link[link], &dev->resource[reg], 
			IORESOURCE_IO, IORESOURCE_IO);
		reg++;
	}

	/* Initialize the memory constraints on the current bus */
	index = amdk8_find_mempair(nodeid, link);
	if (index) {
		dev->resource[reg].base  = 0;
		dev->resource[reg].size  = 0;
		dev->resource[reg].align = log2(HT_MEM_HOST_ALIGN);
		dev->resource[reg].gran  = log2(HT_MEM_HOST_ALIGN);
		dev->resource[reg].limit = 0xffffffffUL;
		dev->resource[reg].flags = IORESOURCE_MEM;
		dev->resource[reg].index = index | (link & 0x3);
		compute_allocate_resource(&dev->link[link], &dev->resource[reg], 
			IORESOURCE_MEM, IORESOURCE_MEM);
		reg++;
	}
	dev->resources = reg;
}

static void amdk8_read_resources(device_t dev)
{
	unsigned nodeid, link;
	nodeid = amdk8_nodeid(dev);
	dev->resources = 0;
	memset(&dev->resource, 0, sizeof(dev->resource));
	for(link = 0; link < dev->links; link++) {
		if (dev->link[link].children) {
			amdk8_link_read_bases(dev, nodeid, link);
		}
	}
}

static void amdk8_set_resource(device_t dev, struct resource *resource, unsigned nodeid)
{
	unsigned long rbase, rlimit;
	unsigned reg, link;

	/* Make certain the resource has actually been set */
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		return;
	}

	/* If I have already stored this resource don't worry about it */
	if (resource->flags & IORESOURCE_STORED) {
		return;
	}
	
	/* Only handle PCI memory and IO resources */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Get the base address */
	rbase = resource->base;
	
	/* Get the limit (rounded up) */
	rlimit = rbase + ((resource->size + resource->align - 1UL) & ~(resource->align -1)) - 1UL;

	/* Get the register and link */
	reg  = resource->index & ~3;
	link = resource->index & 3;

	if (resource->flags & IORESOURCE_IO) {
		uint32_t base, limit;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_IO, IORESOURCE_IO);
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0xfe000fcc;
		base  |= rbase  & 0x01fff000;
		base  |= 3;
		limit &= 0xfe000fc8;
		limit |= rlimit & 0x01fff000;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);

		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
			base |= PCI_IO_BASE_VGA_EN;
		}
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_NO_ISA) {
			base |= PCI_IO_BASE_NO_ISA;
		}
		
		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	else if (resource->flags & IORESOURCE_MEM) {
		uint32_t base, limit;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_MEM, IORESOURCE_MEM);
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0x000000f0;
		base  |= (rbase & 0xffff0000) >> 8;
		base  |= 3;
		limit &= 0x00000048;
		limit |= (rlimit & 0xffff0000) >> 8;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);
		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	resource->flags |= IORESOURCE_STORED;
	printk_debug(
		"%s %02x <- [0x%08lx - 0x%08lx] node %d link %d %s\n",
		dev_path(dev),
		reg, 
		rbase, rlimit,
		nodeid, link,
		(resource->flags & IORESOURCE_IO)? "io": "mem");
}

static void amdk8_set_resources(device_t dev)
{
	unsigned nodeid, link;
	int i;

	/* Find the nodeid */
	nodeid = amdk8_nodeid(dev);	

	/* Set each resource we have found */
	for(i = 0; i < dev->resources; i++) {
		amdk8_set_resource(dev, &dev->resource[i], nodeid);
	}
	
	for(link = 0; link < dev->links; link++) {
		struct bus *bus;
		bus = &dev->link[link];
		if (bus->children) {
			assign_resources(bus);
		}
	}
}

unsigned int amdk8_scan_root_bus(device_t root, unsigned int max)
{
	unsigned reg;
	/* Unmap all of the HT chains */
	for(reg = 0xe0; reg <= 0xec; reg += 4) {
		f1_write_config32(reg, 0);
	}
	max = pci_scan_bus(&root->link[0], PCI_DEVFN(0x18, 0), 0xff, max);
	return max;
}

static void mcf0_control_init(struct device *dev)
{
	uint32_t cmd;

#if 1	
	printk_debug("NB: Function 0 Misc Control.. ");
	/* improve latency and bandwith on HT */
	cmd = pci_read_config32(dev, 0x68);
	cmd &= 0xffff80ff;
	cmd |= 0x00004800;
	pci_write_config32(dev, 0x68, cmd );
#endif

#if 0	
	/* over drive the ht port to 1000 Mhz */
	cmd = pci_read_config32(dev, 0xa8);
	cmd &= 0xfffff0ff;
	cmd |= 0x00000600;
	pci_write_config32(dev, 0xdc, cmd );
#endif	
	printk_debug("done.\n");
}


static void amdk8_enable_resources(struct device *dev)
{
	uint16_t ctrl;
	unsigned link;
	unsigned int vgalink = -1;

	ctrl = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	ctrl |= dev->link[0].bridge_ctrl;
	printk_debug("%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	printk_err("%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, ctrl);

#if 0
	/* let's see what link VGA is on */
	for(link = 0; link < dev->links; link++) {
		device_t child;
		printk_err("Kid %d of k8: bridge ctrl says: 0x%x\n",
			   link, dev->link[link].bridge_ctrl);
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA)
			vgalink = link;
	}

	if (vgalink != 1) {
		/* now find the IOPAIR that goes to vgalink and set the  vga enable in the base part (0x30) */
		/* now allocate an MMIOPAIR and point it to the CPU0, LINK=vgalink */
		/* now set IORR1 so it has a hole for the 0xa0000-0xcffff region */
	}
#endif

	pci_dev_enable_resources(dev);
	//enable_childrens_resources(dev);
}

static struct device_operations northbridge_operations = {
	.read_resources   = amdk8_read_resources,
	.set_resources    = amdk8_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = mcf0_control_init,
	.scan_bus         = amdk8_scan_chains,
	.enable           = 0,
};


static struct pci_driver mcf0_driver __pci_driver = {
	.ops    = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1100,
};


static void enumerate(struct chip *chip)
{
	chip_enumerate(chip);
	chip->dev->ops = &northbridge_operations;
}

struct chip_control northbridge_amd_amdk8_control = {
	.name   = "AMD K8 Northbridge",
	.enumerate = enumerate,
};
