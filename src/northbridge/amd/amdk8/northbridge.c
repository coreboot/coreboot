/* This should be done by Eric
	2004.12 yhlu add dual core support
	2005.01 yhlu add support move apic before pci_domain in MB Config.lb
	2005.02 yhlu add e0 memory hole support
	2005.11 yhlu add put sb ht chain on bus 0
*/

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>

#include <cpu/x86/lapic.h>

#include <cpu/amd/dualcore.h>
#if CONFIG_LOGICAL_CPUS==1
#include <pc80/mc146818rtc.h>
#endif

#include "chip.h"
#include "root_complex/chip.h"
#include "northbridge.h"

#include "amdk8.h"

#include <cpu/amd/model_fxx_rev.h>

#include <cpu/amd/amdk8_sysconf.h>

struct amdk8_sysconf_t sysconf;

#define FX_DEVS 8
static device_t __f0_dev[FX_DEVS];
static device_t __f1_dev[FX_DEVS];

#if 0
static void debug_fx_devs(void)
{
	int i;
	for(i = 0; i < FX_DEVS; i++) {
		device_t dev;
		dev = __f0_dev[i];
		if (dev) {
			printk_debug("__f0_dev[%d]: %s bus: %p\n",
				i, dev_path(dev), dev->bus);
		}
		dev = __f1_dev[i];
		if (dev) {
			printk_debug("__f1_dev[%d]: %s bus: %p\n",
				i, dev_path(dev), dev->bus);
		}
	}
}
#endif

static void get_fx_devs(void)
{
	int i;
	if (__f1_dev[0]) {
		return;
	}
	for(i = 0; i < FX_DEVS; i++) {
		__f0_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 0));
		__f1_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 1));
	}
	if (!__f1_dev[0]) {
		die("Cannot find 0:0x18.1\n");
	}
}

static uint32_t f1_read_config32(unsigned reg)
{
	get_fx_devs();
	return pci_read_config32(__f1_dev[0], reg);
}

static void f1_write_config32(unsigned reg, uint32_t value)
{
	int i;
	get_fx_devs();
	for(i = 0; i < FX_DEVS; i++) {
		device_t dev;
		dev = __f1_dev[i];
		if (dev && dev->enabled) {
			pci_write_config32(dev, reg, value);
		}
	}
}

static unsigned int amdk8_nodeid(device_t dev)
{
	return (dev->path.pci.devfn >> 3) - 0x18;
}

static unsigned int amdk8_scan_chain(device_t dev, unsigned nodeid, unsigned link, unsigned sblink, unsigned int max, unsigned offset_unitid)
{

		uint32_t link_type;
		int i;
		uint32_t busses, config_busses;
		unsigned free_reg, config_reg;
		unsigned ht_unitid_base[4]; // here assume only 4 HT device on chain
		unsigned max_bus;
		unsigned min_bus;
		unsigned max_devfn;

		dev->link[link].cap = 0x80 + (link *0x20);
		do {
			link_type = pci_read_config32(dev, dev->link[link].cap + 0x18);
		} while(link_type & ConnectionPending);
		if (!(link_type & LinkConnected)) {
			return max;
		}
		do {
			link_type = pci_read_config32(dev, dev->link[link].cap + 0x18);
		} while(!(link_type & InitComplete));
		if (!(link_type & NonCoherent)) {
			return max;
		}
		/* See if there is an available configuration space mapping
		 * register in function 1.
		 */
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
		/* If we can't find an available configuration space mapping
		 * register skip this bus
		 */
		if (config_reg > 0xec) {
			return max;
		}

		/* Set up the primary, secondary and subordinate bus numbers.
		 * We have no idea how many busses are behind this bridge yet,
		 * so we set the subordinate bus number to 0xff for the moment.
		 */
#if SB_HT_CHAIN_ON_BUS0 > 0
		// first chain will on bus 0
		if((nodeid == 0) && (sblink==link)) { // actually max is 0 here
			min_bus = max;
		}
	#if SB_HT_CHAIN_ON_BUS0 > 1
		// second chain will be on 0x40, third 0x80, forth 0xc0
		else {
			min_bus = ((max>>6) + 1) * 0x40;
		}
		max = min_bus;
	#else
		//other ...
		else  {
			min_bus = ++max;
		}
	#endif
#else
		min_bus = ++max;
#endif
		max_bus = 0xff;

		dev->link[link].secondary = min_bus;
		dev->link[link].subordinate = max_bus;

		/* Read the existing primary/secondary/subordinate bus
		 * number configuration.
		 */
		busses = pci_read_config32(dev, dev->link[link].cap + 0x14);
		config_busses = f1_read_config32(config_reg);

		/* Configure the bus numbers for this bridge: the configuration
		 * transactions will not be propagates by the bridge if it is
		 * not correctly configured
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

		/* Now we can scan all of the subordinate busses i.e. the
		 * chain on the hypertranport link
		 */
		for(i=0;i<4;i++) {
			ht_unitid_base[i] = 0x20;
		}

		if (min_bus == 0)
			max_devfn = (0x17<<3) | 7;
		else
			max_devfn = (0x1f<<3) | 7;

		max = hypertransport_scan_chain(&dev->link[link], 0, max_devfn, max, ht_unitid_base, offset_unitid);

		/* We know the number of busses behind this bridge.  Set the
		 * subordinate bus number to it's real value
		 */
		dev->link[link].subordinate = max;
		busses = (busses & 0xff00ffff) |
			((unsigned int) (dev->link[link].subordinate) << 16);
		pci_write_config32(dev, dev->link[link].cap + 0x14, busses);

		config_busses = (config_busses & 0x00ffffff) |
			(dev->link[link].subordinate << 24);
		f1_write_config32(config_reg, config_busses);

		{
			// config config_reg, and ht_unitid_base to update hcdn_reg;
			int index;
			unsigned temp = 0;
			index = (config_reg-0xe0) >> 2;
			for(i=0;i<4;i++) {
				temp |= (ht_unitid_base[i] & 0xff) << (i*8);
			}

			sysconf.hcdn_reg[index] = temp;

		}

	return max;
}

static unsigned int amdk8_scan_chains(device_t dev, unsigned int max)
{
	unsigned nodeid;
	unsigned link;
	unsigned sblink = 0;
	unsigned offset_unitid = 0;
	nodeid = amdk8_nodeid(dev);

	if(nodeid==0) {
		sblink = (pci_read_config32(dev, 0x64)>>8) & 3;
#if SB_HT_CHAIN_ON_BUS0 > 0
	#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
		offset_unitid = 1;
	#endif
		max = amdk8_scan_chain(dev, nodeid, sblink, sblink, max, offset_unitid ); // do sb ht chain at first, in case s2885 put sb chain (8131/8111) on link2, but put 8151 on link0
#endif
	}

	for(link = 0; link < dev->links; link++) {
#if SB_HT_CHAIN_ON_BUS0 > 0
		if( (nodeid == 0) && (sblink == link) ) continue; //already done
#endif
		offset_unitid = 0;
		#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
			#if SB_HT_CHAIN_UNITID_OFFSET_ONLY == 1
			if((nodeid == 0) && (sblink == link))
			#endif
				offset_unitid = 1;
		#endif

		max = amdk8_scan_chain(dev, nodeid, link, sblink, max, offset_unitid);
	}

	return max;
}


static int reg_useable(unsigned reg,
	device_t goal_dev, unsigned goal_nodeid, unsigned goal_link)
{
	struct resource *res;
	unsigned nodeid, link=0;
	int result;
	res = 0;
	for(nodeid = 0; !res && (nodeid < FX_DEVS); nodeid++) {
		device_t dev;
		dev = __f0_dev[nodeid];
		if (!dev)
			continue;
		for(link = 0; !res && (link < 3); link++) {
			res = probe_resource(dev, 0x100 + (reg | link));
		}
	}
	result = 2;
	if (res) {
		result = 0;
		if (	(goal_link == (link - 1)) &&
			(goal_nodeid == (nodeid - 1)) &&
			(res->flags <= 1)) {
			result = 1;
		}
	}

	return result;
}

static struct resource *amdk8_find_iopair(device_t dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;
	unsigned free_reg, reg;
	resource = 0;
	free_reg = 0;
	for(reg = 0xc0; reg <= 0xd8; reg += 0x8) {
		int result;
		result = reg_useable(reg, dev, nodeid, link);
		if (result == 1) {
			/* I have been allocated this one */
			break;
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xd8) {
		reg = free_reg;
	}
	if (reg > 0) {
		resource = new_resource(dev, 0x100 + (reg | link));
	}
	return resource;
}

static struct resource *amdk8_find_mempair(device_t dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;
	unsigned free_reg, reg;
	resource = 0;
	free_reg = 0;
	for(reg = 0x80; reg <= 0xb8; reg += 0x8) {
		int result;
		result = reg_useable(reg, dev, nodeid, link);
		if (result == 1) {
			/* I have been allocated this one */
			break;
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xb8) {
		reg = free_reg;
	}
	if (reg > 0) {
		resource = new_resource(dev, 0x100 + (reg | link));
	}
	return resource;
}

static void amdk8_link_read_bases(device_t dev, unsigned nodeid, unsigned link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource =  amdk8_find_iopair(dev, nodeid, link);
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2(HT_IO_HOST_ALIGN);
		resource->gran  = log2(HT_IO_HOST_ALIGN);
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_IO, IORESOURCE_IO);
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran  = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH,
			IORESOURCE_MEM | IORESOURCE_PREFETCH);
	}

	/* Initialize the memory constraints on the current bus */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran  = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM;
		compute_allocate_resource(&dev->link[link], resource,
			IORESOURCE_MEM | IORESOURCE_PREFETCH,
			IORESOURCE_MEM);
	}
}

static void amdk8_read_resources(device_t dev)
{
	unsigned nodeid, link;
	nodeid = amdk8_nodeid(dev);
	for(link = 0; link < dev->links; link++) {
		if (dev->link[link].children) {
			amdk8_link_read_bases(dev, nodeid, link);
		}
	}
}

static void amdk8_set_resource(device_t dev, struct resource *resource, unsigned nodeid)
{
	resource_t rbase, rend;
	unsigned reg, link;
	char buf[50];

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

	/* Ensure I am actually looking at a resource of function 1 */
	if (resource->index < 0x100) {
		return;
	}
	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend  = resource_end(resource);

	/* Get the register and link */
	reg  = resource->index & 0xfc;
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
		limit |= rend & 0x01fff000;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);

		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
			printk_spew("%s, enabling legacy VGA IO forwarding for %s link 0x%x\n",
				    __func__, dev_path(dev), link);
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
			IORESOURCE_MEM | IORESOURCE_PREFETCH,
			resource->flags & (IORESOURCE_MEM | IORESOURCE_PREFETCH));
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0x000000f0;
		base  |= (rbase >> 8) & 0xffffff00;
		base  |= 3;
		limit &= 0x00000048;
		limit |= (rend >> 8) & 0xffffff00;
		limit |= (link & 3) << 4;
		limit |= (nodeid & 7);
		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	resource->flags |= IORESOURCE_STORED;
	sprintf(buf, " <node %d link %d>",
		nodeid, link);
	report_resource_stored(dev, resource, buf);
}

/**
 *
 * I tried to reuse the resource allocation code in amdk8_set_resource()
 * but it is too diffcult to deal with the resource allocation magic.
 */
#if CONFIG_CONSOLE_VGA_MULTI == 1
extern device_t vga_pri;	// the primary vga device, defined in device.c
#endif

static void amdk8_create_vga_resource(device_t dev, unsigned nodeid)
{
	struct resource *resource;
	unsigned link;
	uint32_t base, limit;
	unsigned reg;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = 0; link < dev->links; link++) {
		if (dev->link[link].bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if CONFIG_CONSOLE_VGA_MULTI == 1
			printk_debug("VGA: vga_pri bus num = %d dev->link[link] bus range [%d,%d]\n", vga_pri->bus->secondary,
				dev->link[link].secondary,dev->link[link].subordinate);
			/* We need to make sure the vga_pri is under the link */
			if((vga_pri->bus->secondary >= dev->link[link].secondary ) &&
				(vga_pri->bus->secondary <= dev->link[link].subordinate )
			)
#endif
			break;
		}
	}

	/* no VGA card installed */
	if (link == dev->links)
		return;

	printk_debug("VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, link);

	/* allocate a temp resrouce for legacy VGA buffer */
	resource = amdk8_find_mempair(dev, nodeid, link);
	if(!resource){
		printk_debug("VGA: Can not find free mmio reg for legacy VGA buffer\n");
		return;
	}
	resource->base = 0xa0000;
	resource->size = 0x20000;

	/* write the resource to the hardware */
	reg  = resource->index & 0xfc;
	base  = f1_read_config32(reg);
	limit = f1_read_config32(reg + 0x4);
	base  &= 0x000000f0;
	base  |= (resource->base >> 8) & 0xffffff00;
	base  |= 3;
	limit &= 0x00000048;
	limit |= (resource_end(resource) >> 8) & 0xffffff00;
	limit |= (resource->index & 3) << 4;
	limit |= (nodeid & 7);
	f1_write_config32(reg + 0x4, limit);
	f1_write_config32(reg, base);

	/* release the temp resource */
	resource->flags = 0;
}

static void amdk8_set_resources(device_t dev)
{
	unsigned nodeid, link;
	int i;

	/* Find the nodeid */
	nodeid = amdk8_nodeid(dev);

	amdk8_create_vga_resource(dev, nodeid);

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

static void amdk8_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static void mcf0_control_init(struct device *dev)
{
#if 0
	printk_debug("NB: Function 0 Misc Control.. ");
#endif
#if 0
	printk_debug("done.\n");
#endif
}

static struct device_operations northbridge_operations = {
	.read_resources   = amdk8_read_resources,
	.set_resources    = amdk8_set_resources,
	.enable_resources = amdk8_enable_resources,
	.init             = mcf0_control_init,
	.scan_bus         = amdk8_scan_chains,
	.enable           = 0,
	.ops_pci          = 0,
};


static const struct pci_driver mcf0_driver __pci_driver = {
	.ops    = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1100,
};

struct chip_operations northbridge_amd_amdk8_ops = {
	CHIP_NAME("AMD K8 Northbridge")
	.enable_dev = 0,
};

static void pci_domain_read_resources(device_t dev)
{
	struct resource *resource;
	unsigned reg;

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for(reg = 0x80; reg <= 0xd8; reg+= 0x08) {
		uint32_t base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned nodeid, link;
			device_t reg_dev;
			nodeid = limit & 7;
			link   = (limit >> 4) & 3;
			reg_dev = __f0_dev[nodeid];
			if (reg_dev) {
				/* Reserve the resource  */
				struct resource *reg_resource;
				reg_resource = new_resource(reg_dev, 0x100 + (reg | link));
				if (reg_resource) {
					reg_resource->flags = 1;
				}
			}
		}
	}
#if CONFIG_PCI_64BIT_PREF_MEM == 0
	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->base  = 0x400;
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->limit = 0xfcffffffffULL;
	resource->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
#else
	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, 0);
	resource->base  = 0x400;
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO;
	compute_allocate_resource(&dev->link[0], resource,
		IORESOURCE_IO, IORESOURCE_IO);

	/* Initialize the system wide prefetchable memory resources constraints */
	resource = new_resource(dev, 1);
	resource->limit = 0xfcffffffffULL;
	resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
	compute_allocate_resource(&dev->link[0], resource,
		IORESOURCE_MEM | IORESOURCE_PREFETCH,
		IORESOURCE_MEM | IORESOURCE_PREFETCH);

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, 2);
	resource->limit = 0xfcffffffffULL;
	resource->flags = IORESOURCE_MEM;
	compute_allocate_resource(&dev->link[0], resource,
		IORESOURCE_MEM | IORESOURCE_PREFETCH,
		IORESOURCE_MEM);
#endif
}

static void ram_resource(device_t dev, unsigned long index,
	unsigned long basek, unsigned long sizek)
{
	struct resource *resource;

	if (!sizek) {
		return;
	}
	resource = new_resource(dev, index);
	resource->base  = ((resource_t)basek) << 10;
	resource->size  = ((resource_t)sizek) << 10;
	resource->flags =  IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;
	best = *best_p;
	if (!best || (best->base > new->base)) {
		best = new;
	}
	*best_p = best;
}

static uint32_t find_pci_tolm(struct bus *bus)
{
	struct resource *min;
	uint32_t tolm;
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

#if CONFIG_PCI_64BIT_PREF_MEM == 1
#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM | IORESOURCE_PREFETCH)
#endif

#if HW_MEM_HOLE_SIZEK != 0

struct hw_mem_hole_info {
	unsigned hole_startk;
	int node_id;
};

static struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
		struct hw_mem_hole_info mem_hole;
		int i;

		mem_hole.hole_startk = HW_MEM_HOLE_SIZEK;
		mem_hole.node_id = -1;

		for (i = 0; i < FX_DEVS; i++) {
			uint32_t base;
			uint32_t hole;
			base  = f1_read_config32(0x40 + (i << 3));
			if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
				continue;
			}
			if (!__f1_dev[i])
				continue;
			hole = pci_read_config32(__f1_dev[i], 0xf0);
			if(hole & 1) { // we find the hole
				mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
				mem_hole.node_id = i; // record the node No with hole
				break; // only one hole
			}
		}

		//We need to double check if there is speical set on base reg and limit reg are not continous instead of hole, it will find out it's hole_startk
		if(mem_hole.node_id==-1) {
			uint32_t limitk_pri = 0;
			for(i=0; i<8; i++) {
				uint32_t base, limit;
				unsigned base_k, limit_k;
				base  = f1_read_config32(0x40 + (i << 3));
				if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
					continue;
				}

				base_k = (base & 0xffff0000) >> 2;
				if(limitk_pri != base_k) { // we find the hole
					mem_hole.hole_startk = limitk_pri;
					mem_hole.node_id = i;
					break; //only one hole
				}

				limit = f1_read_config32(0x44 + (i << 3));
				limit_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
				limitk_pri = limit_k;
			}
		}

		return mem_hole;

}
static void disable_hoist_memory(unsigned long hole_startk, int i)
{
	int ii;
	device_t dev;
	uint32_t base, limit;
	uint32_t hoist;
	uint32_t hole_sizek;


	//1. find which node has hole
	//2. change limit in that node.
	//3. change base and limit in later node
	//4. clear that node f0

	//if there is not mem hole enabled, we need to change it's base instead

	hole_sizek = (4*1024*1024) - hole_startk;

	for(ii=7;ii>i;ii--) {

		base  = f1_read_config32(0x40 + (ii << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		limit = f1_read_config32(0x44 + (ii << 3));
		f1_write_config32(0x44 + (ii << 3),limit - (hole_sizek << 2));
		f1_write_config32(0x40 + (ii << 3),base - (hole_sizek << 2));
	}
	limit = f1_read_config32(0x44 + (i << 3));
	f1_write_config32(0x44 + (i << 3),limit - (hole_sizek << 2));
	dev = __f1_dev[i];
	if (dev) {
		hoist = pci_read_config32(dev, 0xf0);
		if(hoist & 1) {
			pci_write_config32(dev, 0xf0, 0);
		} else {
			base = pci_read_config32(dev, 0x40 + (i << 3));
			f1_write_config32(0x40 + (i << 3),base - (hole_sizek << 2));
		}
	}
}

static uint32_t hoist_memory(unsigned long hole_startk, int i)
{
	int ii;
	uint32_t carry_over;
	device_t dev;
	uint32_t base, limit;
	uint32_t basek;
	uint32_t hoist;

	carry_over = (4*1024*1024) - hole_startk;

	for(ii=7;ii>i;ii--) {

		base  = f1_read_config32(0x40 + (ii << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		limit = f1_read_config32(0x44 + (ii << 3));
		f1_write_config32(0x44 + (ii << 3),limit + (carry_over << 2));
		f1_write_config32(0x40 + (ii << 3),base + (carry_over << 2));
	}
	limit = f1_read_config32(0x44 + (i << 3));
	f1_write_config32(0x44 + (i << 3),limit + (carry_over << 2));
	dev = __f1_dev[i];
	base  = pci_read_config32(dev, 0x40 + (i << 3));
	basek  = (base & 0xffff0000) >> 2;
	if(basek == hole_startk) {
		//don't need set memhole here, because hole off set will be 0, overflow
		//so need to change base reg instead, new basek will be 4*1024*1024
		base &= 0x0000ffff;
		base |= (4*1024*1024)<<2;
		f1_write_config32(0x40 + (i<<3), base);
	}
	else if (dev)
	{
		hoist = /* hole start address */
			((hole_startk << 10) & 0xff000000) +
			/* hole address to memory controller address */
			(((basek + carry_over) >> 6) & 0x0000ff00) +
			/* enable */
			1;

		pci_write_config32(dev, 0xf0, hoist);
	}

	return carry_over;
}
#endif

#if HAVE_HIGH_TABLES==1
#define HIGH_TABLES_SIZE 64	// maximum size of high tables in KB
extern uint64_t high_tables_base, high_tables_size;
#endif

static void pci_domain_set_resources(device_t dev)
{
#if CONFIG_PCI_64BIT_PREF_MEM == 1
	struct resource *io, *mem1, *mem2;
	struct resource *resource, *last;
#endif
	unsigned long mmio_basek;
	uint32_t pci_tolm;
	int i, idx;
#if HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
	unsigned reset_memhole = 1;
#endif

#if 0
	/* Place the IO devices somewhere safe */
	io = find_resource(dev, 0);
	io->base = DEVICE_IO_START;
#endif
#if CONFIG_PCI_64BIT_PREF_MEM == 1
	/* Now reallocate the pci resources memory with the
	 * highest addresses I can manage.
	 */
	mem1 = find_resource(dev, 1);
	mem2 = find_resource(dev, 2);

#if 1
	printk_debug("base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
		mem1->base, mem1->limit, mem1->size, mem1->align);
	printk_debug("base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
		mem2->base, mem2->limit, mem2->size, mem2->align);
#endif

	/* See if both resources have roughly the same limits */
	if (((mem1->limit <= 0xffffffff) && (mem2->limit <= 0xffffffff)) ||
		((mem1->limit > 0xffffffff) && (mem2->limit > 0xffffffff)))
	{
		/* If so place the one with the most stringent alignment first
		 */
		if (mem2->align > mem1->align) {
			struct resource *tmp;
			tmp = mem1;
			mem1 = mem2;
			mem2 = tmp;
		}
		/* Now place the memory as high up as it will go */
		mem2->base = resource_max(mem2);
		mem1->limit = mem2->base - 1;
		mem1->base = resource_max(mem1);
	}
	else {
		/* Place the resources as high up as they will go */
		mem2->base = resource_max(mem2);
		mem1->base = resource_max(mem1);
	}

#if 1
	printk_debug("base1: 0x%08Lx limit1: 0x%08Lx size: 0x%08Lx align: %d\n",
		mem1->base, mem1->limit, mem1->size, mem1->align);
	printk_debug("base2: 0x%08Lx limit2: 0x%08Lx size: 0x%08Lx align: %d\n",
		mem2->base, mem2->limit, mem2->size, mem2->align);
#endif

	last = &dev->resource[dev->resources];
	for(resource = &dev->resource[0]; resource < last; resource++)
	{
#if 1
		resource->flags |= IORESOURCE_ASSIGNED;
		resource->flags &= ~IORESOURCE_STORED;
#endif
		compute_allocate_resource(&dev->link[0], resource,
			BRIDGE_IO_MASK, resource->flags & BRIDGE_IO_MASK);

		resource->flags |= IORESOURCE_STORED;
		report_resource_stored(dev, resource, "");

	}
#endif


	pci_tolm = find_pci_tolm(&dev->link[0]);

#warning "FIXME handle interleaved nodes"
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

#if 1
#warning "FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M MMIO hole"
	/* Round the mmio hold to 64M */
	mmio_basek &= ~((64*1024) - 1);
#endif

#if HW_MEM_HOLE_SIZEK != 0
	/* if the hw mem hole is already set in raminit stage, here we will compare mmio_basek and hole_basek
	 * if mmio_basek is bigger that hole_basek and will use hole_basek as mmio_basek and we don't need to reset hole.
	 * otherwise We reset the hole to the mmio_basek
	 */
	#if K8_REV_F_SUPPORT == 0
		if (!is_cpu_pre_e0()) {
	#endif

		mem_hole = get_hw_mem_hole_info();

		if ((mem_hole.node_id !=  -1) && (mmio_basek > mem_hole.hole_startk)) { //We will use hole_basek as mmio_basek, and we don't need to reset hole anymore
			mmio_basek = mem_hole.hole_startk;
			reset_memhole = 0;
		}

		//mmio_basek = 3*1024*1024; // for debug to meet boundary

		if(reset_memhole) {
			if(mem_hole.node_id!=-1) { // We need to select HW_MEM_HOLE_SIZEK for raminit, it can not make hole_startk to some basek too....!
			       // We need to reset our Mem Hole, because We want more big HOLE than we already set
			       //Before that We need to disable mem hole at first, becase memhole could already be set on i+1 instead
				disable_hoist_memory(mem_hole.hole_startk, mem_hole.node_id);
			}

		#if HW_MEM_HOLE_SIZE_AUTO_INC == 1
			//We need to double check if the mmio_basek is valid for hole setting, if it is equal to basek, we need to decrease it some
			uint32_t basek_pri;
			for (i = 0; i < FX_DEVS; i++) {
				uint32_t base;
				uint32_t basek;
				base  = f1_read_config32(0x40 + (i << 3));
				if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
					continue;
				}

				basek = (base & 0xffff0000) >> 2;
				if(mmio_basek == basek) {
					mmio_basek -= (basek - basek_pri)>>1; // increase mem hole size to make sure it is on middle of pri node
					break;
				}
				basek_pri = basek;
			}
		#endif
		}

#if K8_REV_F_SUPPORT == 0
	} // is_cpu_pre_e0
#endif

#endif

	idx = 0x10;
	for(i = 0; i < FX_DEVS; i++) {
		uint32_t base, limit;
		unsigned basek, limitk, sizek;
		base  = f1_read_config32(0x40 + (i << 3));
		limit = f1_read_config32(0x44 + (i << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		basek = (base & 0xffff0000) >> 2;
		limitk = ((limit + 0x00010000) & 0xffff0000) >> 2;
		sizek = limitk - basek;

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < ((8*64)+(8*16))) && (sizek > ((8*64)+(16*16)))) {
			ram_resource(dev, (idx | i), basek, ((8*64)+(8*16)) - basek);
			idx += 0x10;
			basek = (8*64)+(16*16);
			sizek = limitk - ((8*64)+(16*16));

		}


//		printk_debug("node %d : mmio_basek=%08x, basek=%08x, limitk=%08x\n", i, mmio_basek, basek, limitk); //yhlu

		/* See if I need to split the region to accomodate pci memory space */
		if ( (basek < 4*1024*1024 ) && (limitk > mmio_basek) ) {
			if (basek <= mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - basek;
				if(pre_sizek>0) {
					ram_resource(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
#if HAVE_HIGH_TABLES==1
					if (i==0 && high_tables_base==0) {
					/* Leave some space for ACPI, PIRQ and MP tables */
						high_tables_base = (mmio_basek - HIGH_TABLES_SIZE) * 1024;
						high_tables_size = HIGH_TABLES_SIZE * 1024;
						printk_debug("(split)%xK table at =%08llx\n", HIGH_TABLES_SIZE,
							     high_tables_base);
					}
#endif
				}
				#if HW_MEM_HOLE_SIZEK != 0
				if(reset_memhole)
					#if K8_REV_F_SUPPORT == 0
					if(!is_cpu_pre_e0() )
					#endif
		       				 sizek += hoist_memory(mmio_basek,i);
				#endif

				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4*1024*1024) {
				sizek = 0;
			}
			else {
				basek = 4*1024*1024;
				sizek -= (4*1024*1024 - mmio_basek);
			}
		}
		/* If sizek == 0, it was split at mmio_basek without a hole.
		 * Don't create an empty ram_resource.
		 */
		if (sizek)
			ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
#if HAVE_HIGH_TABLES==1
		printk_debug("%d: mmio_basek=%08lx, basek=%08x, limitk=%08x\n",
			     i, mmio_basek, basek, limitk);
		if (i==0 && high_tables_base==0) {
		/* Leave some space for ACPI, PIRQ and MP tables */
			high_tables_base = (limitk - HIGH_TABLES_SIZE) * 1024;
			high_tables_size = HIGH_TABLES_SIZE * 1024;
		}
#endif
	}
	assign_resources(&dev->link[0]);

}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
	unsigned reg;
	int i;
	/* Unmap all of the HT chains */
	for(reg = 0xe0; reg <= 0xec; reg += 4) {
		f1_write_config32(reg, 0);
	}
	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0x18, 0), 0xff, max);

	/* Tune the hypertransport transaction for best performance.
	 * Including enabling relaxed ordering if it is safe.
	 */
	get_fx_devs();
	for(i = 0; i < FX_DEVS; i++) {
		device_t f0_dev;
		f0_dev = __f0_dev[i];
		if (f0_dev && f0_dev->enabled) {
			uint32_t httc;
			httc = pci_read_config32(f0_dev, HT_TRANSACTION_CONTROL);
			httc &= ~HTTC_RSP_PASS_PW;
			if (!dev->link[0].disable_relaxed_ordering) {
				httc |= HTTC_RSP_PASS_PW;
			}
			printk_spew("%s passpw: %s\n",
				dev_path(dev),
				(!dev->link[0].disable_relaxed_ordering)?
				"enabled":"disabled");
			pci_write_config32(f0_dev, HT_TRANSACTION_CONTROL, httc);
		}
	}
	return max;
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = enable_childrens_resources,
	.init             = 0,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus      = &pci_cf8_conf1,
};

static unsigned int cpu_bus_scan(device_t dev, unsigned int max)
{
	struct bus *cpu_bus;
	device_t dev_mc;
	int bsp_apicid;
	int i,j;
	unsigned nb_cfg_54;
	unsigned siblings;
	int e0_later_single_core;
	uint32_t disable_siblings;

	nb_cfg_54 = 0;
	sysconf.enabled_apic_ext_id = 0;
	sysconf.lift_bsp_apicid = 0;
	siblings = 0;

	/* Find the bootstrap processors apicid */
	bsp_apicid = lapicid();
	sysconf.apicid_offset = bsp_apicid;

	disable_siblings = !CONFIG_LOGICAL_CPUS;
#if CONFIG_LOGICAL_CPUS == 1
	get_option("dual_core", &disable_siblings);
#endif

	// for pre_e0, nb_cfg_54 can not be set, ( even set, when you read it still be 0)
	// How can I get the nb_cfg_54 of every node' nb_cfg_54 in bsp??? and differ d0 and e0 single core

	nb_cfg_54 = read_nb_cfg_54();

	dev_mc = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	if (!dev_mc) {
		die("0:18.0 not found?");
	}

	sysconf.nodes = ((pci_read_config32(dev_mc, 0x60)>>4) & 7) + 1;


	if (pci_read_config32(dev_mc, 0x68) & (HTTC_APIC_EXT_ID|HTTC_APIC_EXT_BRD_CST))
	{
		sysconf.enabled_apic_ext_id = 1;
		if(bsp_apicid == 0) {
			/* bsp apic id is not changed */
			sysconf.apicid_offset = APIC_ID_OFFSET;
		} else
		{
			sysconf.lift_bsp_apicid = 1;
		}

	}

	/* Find which cpus are present */
	cpu_bus = &dev->link[0];
	for(i = 0; i < sysconf.nodes; i++) {
		device_t cpu_dev, cpu;
		struct device_path cpu_path;

		/* Find the cpu's pci device */
		cpu_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));
		if (!cpu_dev) {
			/* If I am probing things in a weird order
			 * ensure all of the cpu's pci devices are found.
			 */
			int local_j;
			device_t dev_f0;
			for(local_j = 0; local_j <= 3; local_j++) {
				cpu_dev = pci_probe_dev(NULL, dev_mc->bus,
					PCI_DEVFN(0x18 + i, local_j));
			}
			/* Ok, We need to set the links for that device.
			 * otherwise the device under it will not be scanned
			 */
			dev_f0 = dev_find_slot(0, PCI_DEVFN(0x18+i,0));
			if(dev_f0) {
				dev_f0->links = 3;
				for(local_j=0;local_j<3;local_j++) {
					dev_f0->link[local_j].link = local_j;
					dev_f0->link[local_j].dev = dev_f0;
				}
			}

		}

		e0_later_single_core = 0;
		if (cpu_dev && cpu_dev->enabled) {
			j = pci_read_config32(cpu_dev, 0xe8);
			j = (j >> 12) & 3; // dev is func 3
			printk_debug("  %s siblings=%d\n", dev_path(cpu_dev), j);

			if(nb_cfg_54) {
				// For e0 single core if nb_cfg_54 is set, apicid will be 0, 2, 4....
				//  ----> you can mixed single core e0 and dual core e0 at any sequence
				// That is the typical case

				if(j == 0 ){
				       #if K8_REV_F_SUPPORT == 0
		 		       	e0_later_single_core = is_e0_later_in_bsp(i);  // single core
				       #else
				       	e0_later_single_core = is_cpu_f0_in_bsp(i);  // We can read cpuid(1) from Func3
				       #endif
				} else {
				       e0_later_single_core = 0;
	       			}
				if(e0_later_single_core) {
					printk_debug("\tFound Rev E or Rev F later single core\r\n");

					j=1;
				}

				if(siblings > j ) {
				}
				else {
					siblings = j;
				}
			} else {
				siblings = j;
  			}
		}

		unsigned jj;
		if(e0_later_single_core || disable_siblings) {
			jj = 0;
		} else
		{
			jj = siblings;
		}
#if 0
		jj = 0; // if create cpu core1 path in amd_siblings by core0
#endif

		for (j = 0; j <=jj; j++ ) {

			/* Build the cpu device path */
			cpu_path.type = DEVICE_PATH_APIC;
			cpu_path.apic.apic_id = i * (nb_cfg_54?(siblings+1):1) + j * (nb_cfg_54?1:8);

			/* See if I can find the cpu */
			cpu = find_dev_path(cpu_bus, &cpu_path);

			/* Enable the cpu if I have the processor */
			if (cpu_dev && cpu_dev->enabled) {
				if (!cpu) {
					cpu = alloc_dev(cpu_bus, &cpu_path);
				}
				if (cpu) {
					cpu->enabled = 1;
				}
			}

			/* Disable the cpu if I don't have the processor */
			if (cpu && (!cpu_dev || !cpu_dev->enabled)) {
				cpu->enabled = 0;
			}

			/* Report what I have done */
			if (cpu) {
				cpu->path.apic.node_id = i;
				cpu->path.apic.core_id = j;
				if(sysconf.enabled_apic_ext_id) {
					if(sysconf.lift_bsp_apicid) {
						cpu->path.apic.apic_id += sysconf.apicid_offset;
					} else
					{
					       if (cpu->path.apic.apic_id != 0)
						       cpu->path.apic.apic_id += sysconf.apicid_offset;
					}
				}
				printk_debug("CPU: %s %s\n",
					dev_path(cpu), cpu->enabled?"enabled":"disabled");
			}

		} //j
	}
	return max;
}

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(&dev->link[0]);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources   = cpu_bus_noop,
	.set_resources    = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init             = cpu_bus_init,
	.scan_bus         = cpu_bus_scan,
};

static void root_complex_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_amd_amdk8_root_complex_ops = {
	CHIP_NAME("AMD K8 Root Complex")
	.enable_dev = root_complex_enable_dev,
};
