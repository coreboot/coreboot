/* This should be done by Eric
	2004.12 yhlu add dual core support
	2005.01 yhlu add support move apic before pci_domain in MB devicetree.cb
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
#include <lib.h>
#include <cpu/cpu.h>
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
#include <arch/acpi.h>
#include "acpi.h"
#endif

#include <cpu/x86/lapic.h>
#include <cpu/amd/mtrr.h>

#include <cpu/amd/multicore.h>
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
#include <pc80/mc146818rtc.h>
#endif

#include "northbridge.h"

#include "amdk8.h"

#include <cpu/amd/model_fxx_rev.h>

#include <cpu/amd/amdk8_sysconf.h>

struct amdk8_sysconf_t sysconf;

#define MAX_FX_DEVS 8
static struct device *__f0_dev[MAX_FX_DEVS];
static struct device *__f1_dev[MAX_FX_DEVS];
static unsigned fx_devs = 0;

static void get_fx_devs(void)
{
	int i;
	for (i = 0; i < MAX_FX_DEVS; i++) {
		__f0_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 0));
		__f1_dev[i] = dev_find_slot(0, PCI_DEVFN(0x18 + i, 1));
		if (__f0_dev[i] != NULL && __f1_dev[i] != NULL)
			fx_devs = i+1;
	}
	if (__f1_dev[0] == NULL || __f0_dev[0] == NULL || fx_devs == 0) {
		die("Cannot find 0:0x18.[0|1]\n");
	}
}

static u32 f1_read_config32(unsigned reg)
{
	if (fx_devs == 0)
		get_fx_devs();
	return pci_read_config32(__f1_dev[0], reg);
}

static void f1_write_config32(unsigned reg, u32 value)
{
	int i;
	if (fx_devs == 0)
		get_fx_devs();
	for (i = 0; i < fx_devs; i++) {
		struct device *dev;
		dev = __f1_dev[i];
		if (dev && dev->enabled) {
			pci_write_config32(dev, reg, value);
		}
	}
}

typedef enum {
	HT_ROUTE_CLOSE,
	HT_ROUTE_SCAN,
	HT_ROUTE_FINAL,
} scan_state;

static void ht_route_link(struct bus *link, scan_state mode)
{
	struct device *dev = link->dev;
	struct bus *parent = dev->bus;
	u32 busses;

	if (mode == HT_ROUTE_SCAN) {
		if (link->dev->bus->subordinate == 0)
			link->secondary = 0;
		else
			link->secondary = parent->subordinate + 1;

		link->subordinate = link->secondary;
	}

	/* Configure the bus numbers for this bridge: the configuration
	 * transactions will not be propagated by the bridge if it is
	 * not correctly configured
	 */
	busses = pci_read_config32(link->dev, link->cap + 0x14);
	busses &= 0xff000000;
	busses |= parent->secondary & 0xff;
	if (mode == HT_ROUTE_CLOSE) {
		busses |= 0xfeff << 8;
	} else if (mode == HT_ROUTE_SCAN) {
		busses |= ((u32) link->secondary & 0xff) << 8;
		busses |= 0xff << 16;
	} else if (mode == HT_ROUTE_FINAL) {
		busses |= ((u32) link->secondary & 0xff) << 8;
		busses |= ((u32) link->subordinate & 0xff) << 16;
	}
	pci_write_config32(link->dev, link->cap + 0x14, busses);

	if (mode == HT_ROUTE_FINAL) {
		/* Second chain will be on 0x40, third 0x80, forth 0xc0. */
		if (CONFIG_HT_CHAIN_DISTRIBUTE)
			parent->subordinate = ALIGN_UP(link->subordinate, 0x40) - 1;
		else
			parent->subordinate = link->subordinate;
	}
}

static u32 amdk8_nodeid(struct device *dev)
{
	return (dev->path.pci.devfn >> 3) - 0x18;
}

static void amdk8_scan_chain(struct bus *link)
{
		unsigned int next_unitid;
		int index;
		u32 config_busses;
		u32 free_reg, config_reg;
		u32 nodeid = amdk8_nodeid(link->dev);

		/* See if there is an available configuration space mapping
		 * register in function 1.
		 */
		free_reg = 0;
		for (config_reg = 0xe0; config_reg <= 0xec; config_reg += 4) {
			u32 config;
			config = f1_read_config32(config_reg);
			if (!free_reg && ((config & 3) == 0)) {
				free_reg = config_reg;
				continue;
			}
			if (((config & 3) == 3) &&
				(((config >> 4) & 7) == nodeid) &&
				(((config >> 8) & 3) == link->link_num)) {
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
			return;
		}

		/* Set up the primary, secondary and subordinate bus numbers.
		 * We have no idea how many busses are behind this bridge yet,
		 * so we set the subordinate bus number to 0xff for the moment.
		 */

		ht_route_link(link, HT_ROUTE_SCAN);

		config_busses = f1_read_config32(config_reg);
		config_busses &= 0x000fc88;
		config_busses |=
			(3 << 0) |  /* rw enable, no device compare */
			((nodeid & 7) << 4) |
			((link->link_num & 3) << 8) |
			((link->secondary) << 16) |
			(0xff << 24);
		f1_write_config32(config_reg, config_busses);

		/* Now we can scan all of the subordinate busses i.e. the
		 * chain on the hypertranport link
		 */

		next_unitid = hypertransport_scan_chain(link);

		/* Now that nothing is overlapping it is safe to scan the children. */
		pci_scan_bus(link, 0x00, ((next_unitid - 1) << 3) | 7);

		/* We know the number of busses behind this bridge.  Set the
		 * subordinate bus number to it's real value
		 */

		ht_route_link(link, HT_ROUTE_FINAL);

		config_busses = (config_busses & 0x00ffffff) |
			(link->subordinate << 24);
		f1_write_config32(config_reg, config_busses);

		index = (config_reg-0xe0) >> 2;
		sysconf.hcdn_reg[index] = link->hcdn_reg;
}

/* Do sb ht chain at first, in case s2885 put sb chain
 * (8131/8111) on link2, but put 8151 on link0.
 */
static void relocate_sb_ht_chain(void)
{
	struct device *dev;
	struct bus *link, *prev = NULL;
	u8 sblink;

	dev = dev_find_slot(CONFIG_CBB, PCI_DEVFN(CONFIG_CDB, 0));
	sblink = (pci_read_config32(dev, 0x64)>>8) & 3;
	link = dev->link_list;

	while (link) {
		if (link->link_num == sblink) {
			if (!prev)
				return;
			prev->next = link->next;
			link->next = dev->link_list;
			dev->link_list = link;
			return;
		}
		prev = link;
		link = link->next;
	}
}

static void trim_ht_chain(struct device *dev)
{
	struct bus *link;

	/* Check for connected links. */
	for (link = dev->link_list; link; link = link->next) {
		link->cap = 0x80 + (link->link_num * 0x20);
		link->ht_link_up = ht_is_non_coherent_link(link);
	}
}

static void amdk8_scan_chains(struct device *dev)
{
	struct bus *link;

	trim_ht_chain(dev);

	for (link = dev->link_list; link; link = link->next) {
		if (link->ht_link_up)
			amdk8_scan_chain(link);
	}
}


static int reg_useable(unsigned reg, struct device *goal_dev,
		       unsigned goal_nodeid, unsigned goal_link)
{
	struct resource *res;
	unsigned nodeid, link = 0;
	int result;
	res = 0;
	for (nodeid = 0; !res && (nodeid < fx_devs); nodeid++) {
		struct device *dev;
		dev = __f0_dev[nodeid];
		if (!dev)
			continue;
		for (link = 0; !res && (link < 3); link++) {
			res = probe_resource(dev, IOINDEX(0x100 + reg, link));
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

static unsigned amdk8_find_reg(struct device *dev, unsigned nodeid,
			       unsigned link, unsigned min, unsigned max)
{
	unsigned resource;
	unsigned free_reg, reg;
	resource = 0;
	free_reg = 0;
	for (reg = min; reg <= max; reg += 0x8) {
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
	if (reg > max) {
		reg = free_reg;
	}
	if (reg > 0) {
		resource = IOINDEX(0x100 + reg, link);
	}
	return resource;
}

static unsigned amdk8_find_iopair(struct device *dev, unsigned nodeid,
				  unsigned link)
{
	return amdk8_find_reg(dev, nodeid, link, 0xc0, 0xd8);
}

static unsigned amdk8_find_mempair(struct device *dev, unsigned nodeid,
				   unsigned link)
{
	return amdk8_find_reg(dev, nodeid, link, 0x80, 0xb8);
}

static void amdk8_link_read_bases(struct device *dev, unsigned nodeid,
				  unsigned link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource = new_resource(dev, IOINDEX(0, link));
	if (resource) {
		resource->base  = 0;
		resource->size  = 0;
		resource->align = log2(HT_IO_HOST_ALIGN);
		resource->gran  = log2(HT_IO_HOST_ALIGN);
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = new_resource(dev, IOINDEX(2, link));
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_PREFETCH;
		resource->flags |= IORESOURCE_BRIDGE;
	}

	/* Initialize the memory constraints on the current bus */
	resource = new_resource(dev, IOINDEX(1, link));
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
	}
}

static void amdk8_create_vga_resource(struct device *dev, unsigned nodeid);

static void amdk8_read_resources(struct device *dev)
{
	unsigned nodeid;
	struct bus *link;
	nodeid = amdk8_nodeid(dev);
	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			amdk8_link_read_bases(dev, nodeid, link->link_num);
		}
	}
	amdk8_create_vga_resource(dev, nodeid);
}

static void amdk8_set_resource(struct device *dev, struct resource *resource,
			       unsigned nodeid)
{
	struct bus *link;
	resource_t rbase, rend;
	unsigned reg, link_num;
	char buf[50];

	/* Make certain the resource has actually been set */
	if (!(resource->flags & IORESOURCE_ASSIGNED)) {
		printk(BIOS_ERR, "%s: can't set unassigned resource @%lx %lx\n",
			   __func__, resource->index, resource->flags);
		return;
	}

	/* If I have already stored this resource don't worry about it */
	if (resource->flags & IORESOURCE_STORED) {
		printk(BIOS_ERR, "%s: can't set stored resource @%lx %lx\n", __func__,
			   resource->index, resource->flags);
		return;
	}

	/* Only handle PCI memory and IO resources */
	if (!(resource->flags & (IORESOURCE_MEM | IORESOURCE_IO)))
		return;

	/* Ensure I am actually looking at a resource of function 1 */
	if (resource->index < 0x100) {
		return;
	}

	if (resource->size == 0)
		return;

	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend  = resource_end(resource);

	/* Get the register and link */
	reg  = resource->index & 0xfc;
	link_num = IOINDEX_LINK(resource->index);

	for (link = dev->link_list; link; link = link->next)
		if (link->link_num == link_num)
			break;

	if (link == NULL) {
		printk(BIOS_ERR, "%s: can't find link %x for %lx\n", __func__,
			   link_num, resource->index);
		return;
	}

	if (resource->flags & IORESOURCE_IO) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0xfe000fcc;
		base  |= rbase  & 0x01fff000;
		base  |= 3;
		limit &= 0xfe000fc8;
		limit |= rend & 0x01fff000;
		limit |= (link_num & 3) << 4;
		limit |= (nodeid & 7);

		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
			printk(BIOS_SPEW, "%s, enabling legacy VGA IO forwarding for %s link 0x%x\n",
				    __func__, dev_path(dev), link_num);
			base |= PCI_IO_BASE_VGA_EN;
		}
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_NO_ISA) {
			base |= PCI_IO_BASE_NO_ISA;
		}

		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	else if (resource->flags & IORESOURCE_MEM) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x4);
		base  &= 0x000000f0;
		base  |= (rbase >> 8) & 0xffffff00;
		base  |= 3;
		limit &= 0x00000048;
		limit |= (rend >> 8) & 0xffffff00;
		limit |= (link_num & 3) << 4;
		limit |= (nodeid & 7);
		f1_write_config32(reg + 0x4, limit);
		f1_write_config32(reg, base);
	}
	resource->flags |= IORESOURCE_STORED;
	snprintf(buf, sizeof(buf), " <node %x link %x>",
		 nodeid, link_num);
	report_resource_stored(dev, resource, buf);
}

static void amdk8_create_vga_resource(struct device *dev, unsigned nodeid)
{
	struct resource *resource;
	struct bus *link;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list; link; link = link->next) {
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if IS_ENABLED(CONFIG_MULTIPLE_VGA_ADAPTERS)
			extern struct device *vga_pri; // the primary vga device, defined in device.c
			printk(BIOS_DEBUG, "VGA: vga_pri bus num = %d link bus range [%d,%d]\n", vga_pri->bus->secondary,
				link->secondary,link->subordinate);
			/* We need to make sure the vga_pri is under the link */
			if ((vga_pri->bus->secondary >= link->secondary) &&
				(vga_pri->bus->secondary <= link->subordinate)
			)
#endif
			break;
		}
	}

	/* no VGA card installed */
	if (link == NULL)
		return;

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, link->link_num);

	/* allocate a temp resource for the legacy VGA buffer */
	resource = new_resource(dev, IOINDEX(4, link->link_num));
	if (!resource) {
		printk(BIOS_DEBUG, "VGA: %s out of resources.\n", dev_path(dev));
		return;
	}
	resource->base = 0xa0000;
	resource->size = 0x20000;
	resource->limit = 0xffffffff;
	resource->flags = IORESOURCE_FIXED | IORESOURCE_MEM |
			  IORESOURCE_ASSIGNED;
}

static void amdk8_set_resources(struct device *dev)
{
	unsigned nodeid;
	struct bus *bus;
	struct resource *res;

	/* Find the nodeid */
	nodeid = amdk8_nodeid(dev);

	/* Set each resource we have found */
	for (res = dev->resource_list; res; res = res->next) {
		struct resource *old = NULL;
		unsigned index;

		if (res->size == 0) /* No need to allocate registers. */
			continue;

		if (res->flags & IORESOURCE_IO)
			index = amdk8_find_iopair(dev, nodeid,
						  IOINDEX_LINK(res->index));
		else
			index = amdk8_find_mempair(dev, nodeid,
						   IOINDEX_LINK(res->index));

		old = probe_resource(dev, index);
		if (old) {
			res->index = old->index;
			old->index = 0;
			old->flags = 0;
		}
		else
			res->index = index;

		amdk8_set_resource(dev, res, nodeid);
	}

	compact_resources(dev);

	for (bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children) {
			assign_resources(bus);
		}
	}
}

static void mcf0_control_init(struct device *dev)
{
}

static struct device_operations northbridge_operations = {
	.read_resources	  = amdk8_read_resources,
	.set_resources	  = amdk8_set_resources,
	.enable_resources = pci_dev_enable_resources,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = k8acpi_write_vars,
	.write_acpi_tables = northbridge_write_acpi_tables,
#endif
	.init		  = mcf0_control_init,
	.scan_bus	  = amdk8_scan_chains,
	.enable		  = 0,
	.ops_pci	  = 0,
};


static const struct pci_driver mcf0_driver __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1100,
};

static void amdk8_nb_init(void *chip_info)
{
	relocate_sb_ht_chain();
}

struct chip_operations northbridge_amd_amdk8_ops = {
	CHIP_NAME("AMD K8 Northbridge")
	.enable_dev = 0,
	.init = amdk8_nb_init,
};

static void amdk8_domain_read_resources(struct device *dev)
{
	unsigned reg;

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for (reg = 0x80; reg <= 0xd8; reg+= 0x08) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned nodeid, reg_link;
			struct device *reg_dev;
			nodeid = limit & 7;
			reg_link = (limit >> 4) & 3;
			reg_dev = __f0_dev[nodeid];
			if (reg_dev) {
				/* Reserve the resource  */
				struct resource *res;
				res = new_resource(reg_dev, IOINDEX(0x100 + reg, reg_link));
				if (res) {
					res->base = base;
					res->limit = limit;
					res->flags = 1;
				}
			}
		}
	}

	pci_domain_read_resources(dev);
}

static void my_tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;
	best = *best_p;
	/* Skip VGA. */
	if (!best || (best->base > new->base && new->base > 0xa0000)) {
		best = new;
	}
	*best_p = best;
}

static u32 my_find_pci_tolm(struct bus *bus)
{
	struct resource *min;
	u32 tolm;
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, my_tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0

struct hw_mem_hole_info {
	unsigned hole_startk;
	int node_id;
};

static struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
		struct hw_mem_hole_info mem_hole;
		int i;

		mem_hole.hole_startk = CONFIG_HW_MEM_HOLE_SIZEK;
		mem_hole.node_id = -1;

		for (i = 0; i < fx_devs; i++) {
			u32 base;
			u32 hole;
			base  = f1_read_config32(0x40 + (i << 3));
			if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
				continue;
			}

			hole = pci_read_config32(__f1_dev[i], 0xf0);
			if (hole & 1) { // we find the hole
				mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
				mem_hole.node_id = i; // record the node No with hole
				break; // only one hole
			}
		}

		/* We need to double check if there is special set on base reg and limit reg
		 * are not continuous instead of hole, it will find out its hole_startk.
		 */
		if (mem_hole.node_id==-1) {
			u32 limitk_pri = 0;
			for (i = 0; i < 8; i++) {
				u32 base, limit;
				unsigned base_k, limit_k;
				base  = f1_read_config32(0x40 + (i << 3));
				if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
					continue;
				}

				base_k = (base & 0xffff0000) >> 2;
				if (limitk_pri != base_k) { // we find the hole
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

static void disable_hoist_memory(unsigned long hole_startk, int node_id)
{
	int i;
	struct device *dev;
	u32 base, limit;
	u32 hoist;
	u32 hole_sizek;


	//1. find which node has hole
	//2. change limit in that node.
	//3. change base and limit in later node
	//4. clear that node f0

	//if there is not mem hole enabled, we need to change it's base instead

	hole_sizek = (4*1024*1024) - hole_startk;

	for (i = 7; i > node_id; i--) {

		base  = f1_read_config32(0x40 + (i << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		limit = f1_read_config32(0x44 + (i << 3));
		f1_write_config32(0x44 + (i << 3),limit - (hole_sizek << 2));
		f1_write_config32(0x40 + (i << 3),base - (hole_sizek << 2));
	}
	limit = f1_read_config32(0x44 + (node_id << 3));
	f1_write_config32(0x44 + (node_id << 3),limit - (hole_sizek << 2));
	dev = __f1_dev[node_id];
	if (dev == NULL) {
		printk(BIOS_ERR, "%s: node %x is NULL!\n", __func__, node_id);
		return;
	}
	hoist = pci_read_config32(dev, 0xf0);
	if (hoist & 1) {
		pci_write_config32(dev, 0xf0, 0);
	} else {
		base = pci_read_config32(dev, 0x40 + (node_id << 3));
		f1_write_config32(0x40 + (node_id << 3),base - (hole_sizek << 2));
	}
}

static u32 hoist_memory(unsigned long hole_startk, int node_id)
{
	int i;
	u32 carry_over;
	struct device *dev;
	u32 base, limit;
	u32 basek;
	u32 hoist;

	carry_over = (4*1024*1024) - hole_startk;

	for (i = 7; i > node_id; i--) {

		base  = f1_read_config32(0x40 + (i << 3));
		if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
			continue;
		}
		limit = f1_read_config32(0x44 + (i << 3));
		f1_write_config32(0x44 + (i << 3),limit + (carry_over << 2));
		f1_write_config32(0x40 + (i << 3),base + (carry_over << 2));
	}
	limit = f1_read_config32(0x44 + (node_id << 3));
	f1_write_config32(0x44 + (node_id << 3),limit + (carry_over << 2));
	dev = __f1_dev[node_id];
	base  = pci_read_config32(dev, 0x40 + (node_id << 3));
	basek  = (base & 0xffff0000) >> 2;
	if (basek == hole_startk) {
		//don't need set memhole here, because hole off set will be 0, overflow
		//so need to change base reg instead, new basek will be 4*1024*1024
		base &= 0x0000ffff;
		base |= (4*1024*1024)<<2;
		f1_write_config32(0x40 + (node_id<<3), base);
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

#include <cbmem.h>

static void setup_uma_memory(void)
{
#if IS_ENABLED(CONFIG_GFXUMA)
	uint32_t topmem = (uint32_t) bsp_topmem();

#if !IS_ENABLED(CONFIG_BOARD_ASROCK_939A785GMH) && \
	!IS_ENABLED(CONFIG_BOARD_AMD_MAHOGANY)

	switch (topmem) {
	case 0x10000000:	/* 256M system memory */
		uma_memory_size = 0x2000000;	/* 32M recommended UMA */
		break;

	case 0x18000000:	/* 384M system memory */
		uma_memory_size = 0x4000000;	/* 64M recommended UMA */
		break;

	case 0x20000000:	/* 512M system memory */
		uma_memory_size = 0x4000000;	/* 64M recommended UMA */
		break;

	default:		/* 1GB and above system memory */
		uma_memory_size = 0x8000000;	/* 128M recommended UMA */
		break;
	}
#else
	/* refer to UMA Size Consideration in 780 BDG. */
	switch (topmem) {
	case 0x10000000:	/* 256M system memory */
		uma_memory_size = 0x4000000;	/* 64M recommended UMA */
		break;

	case 0x20000000:	/* 512M system memory */
		uma_memory_size = 0x8000000;	/* 128M recommended UMA */
		break;

	default:		/* 1GB and above system memory */
		uma_memory_size = 0x10000000;	/* 256M recommended UMA */
		break;
	}
#endif

	uma_memory_base = topmem - uma_memory_size;	/* TOP_MEM1 */
	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
		    __func__, uma_memory_size, uma_memory_base);
#endif
}

static void amdk8_domain_set_resources(struct device *dev)
{
	unsigned long mmio_basek;
	u32 pci_tolm;
	u64 ramtop = 0;
	int i, idx;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
	u32 reset_memhole = 1;
#endif

	pci_tolm = my_find_pci_tolm(dev->link_list);

	// FIXME handle interleaved nodes. If you fix this here, please fix
	// amdfam10, too.
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

	// FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M
	// MMIO hole. If you fix this here, please fix amdfam10, too.
	/* Round the mmio hole to 64M */
	mmio_basek &= ~((64*1024) - 1);

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	/* if the hw mem hole is already set in raminit stage, here we will compare mmio_basek and hole_basek
	 * if mmio_basek is bigger that hole_basek and will use hole_basek as mmio_basek and we don't need to reset hole.
	 * otherwise We reset the hole to the mmio_basek
	 */
	#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		if (!is_cpu_pre_e0()) {
	#endif

		mem_hole = get_hw_mem_hole_info();

		if ((mem_hole.node_id !=  -1) && (mmio_basek > mem_hole.hole_startk)) { //We will use hole_basek as mmio_basek, and we don't need to reset hole anymore
			mmio_basek = mem_hole.hole_startk;
			reset_memhole = 0;
		}

		if (reset_memhole) {
			if (mem_hole.node_id!=-1) { // We need to select CONFIG_HW_MEM_HOLE_SIZEK for raminit, it can not make hole_startk to some basek too....!
			       // We need to reset our Mem Hole, because We want more big HOLE than we already set
			       //Before that We need to disable mem hole at first, becase memhole could already be set on i+1 instead
				disable_hoist_memory(mem_hole.hole_startk, mem_hole.node_id);
			}

		#if IS_ENABLED(CONFIG_HW_MEM_HOLE_SIZE_AUTO_INC)
			//We need to double check if the mmio_basek is valid for hole setting, if it is equal to basek, we need to decrease it some
			u32 basek_pri;
			for (i = 0; i < fx_devs; i++) {
				u32 base;
				u32 basek;
				base  = f1_read_config32(0x40 + (i << 3));
				if ((base & ((1<<1)|(1<<0))) != ((1<<1)|(1<<0))) {
					continue;
				}

				basek = (base & 0xffff0000) >> 2;
				if (mmio_basek == basek) {
					mmio_basek -= (basek - basek_pri)>>1; // increase mem hole size to make sure it is on middle of pri node
					break;
				}
				basek_pri = basek;
			}
		#endif
		}

#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
	} // is_cpu_pre_e0
#endif

#endif

	idx = 0x10;
	for (i = 0; i < fx_devs; i++) {
		u32 base, limit;
		u32 basek, limitk, sizek;
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


#if IS_ENABLED(CONFIG_GFXUMA)
		printk(BIOS_DEBUG, "node %d : uma_memory_base/1024=0x%08llx, mmio_basek=0x%08lx, basek=0x%08x, limitk=0x%08x\n", i, uma_memory_base >> 10, mmio_basek, basek, limitk);
		if ((uma_memory_base >> 10) < mmio_basek)
			printk(BIOS_ALERT, "node %d: UMA memory starts below mmio_basek\n", i);
#endif

		/* See if I need to split the region to accommodate pci memory space */
		if ((basek < 4*1024*1024) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek > 0) {
					ram_resource(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
					if (!ramtop)
						ramtop = mmio_basek * 1024;
				}
				#if CONFIG_HW_MEM_HOLE_SIZEK != 0
				if (reset_memhole)
					#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
					if (!is_cpu_pre_e0())
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

		ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
		printk(BIOS_DEBUG, "%d: mmio_basek=%08lx, basek=%08x, limitk=%08x\n",
			     i, mmio_basek, basek, limitk);
		if (!ramtop)
			ramtop = limitk * 1024;
	}

#if IS_ENABLED(CONFIG_GFXUMA)
	set_late_cbmem_top(uma_memory_base);
	uma_resource(dev, 7, uma_memory_base >> 10, uma_memory_size >> 10);
#else
	set_late_cbmem_top(ramtop);
#endif
	assign_resources(dev->link_list);

}

static void amdk8_domain_scan_bus(struct device *dev)
{
	u32 reg;
	int i;
	struct bus *link = dev->link_list;

	/* Unmap all of the HT chains */
	for (reg = 0xe0; reg <= 0xec; reg += 4) {
		f1_write_config32(reg, 0);
	}

	link->secondary = dev->bus->subordinate;
	pci_scan_bus(link, PCI_DEVFN(0x18, 0), 0xff);
	dev->bus->subordinate = link->subordinate;

	/* Tune the hypertransport transaction for best performance.
	 * Including enabling relaxed ordering if it is safe.
	 */
	get_fx_devs();
	for (i = 0; i < fx_devs; i++) {
		struct device *f0_dev;
		f0_dev = __f0_dev[i];
		if (f0_dev && f0_dev->enabled) {
			u32 httc;
			httc = pci_read_config32(f0_dev, HT_TRANSACTION_CONTROL);
			httc &= ~HTTC_RSP_PASS_PW;
			if (!dev->link_list->disable_relaxed_ordering) {
				httc |= HTTC_RSP_PASS_PW;
			}
			printk(BIOS_SPEW, "%s passpw: %s\n",
				dev_path(dev),
				(!dev->link_list->disable_relaxed_ordering)?
				"enabled":"disabled");
			pci_write_config32(f0_dev, HT_TRANSACTION_CONTROL, httc);
		}
	}
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = amdk8_domain_read_resources,
	.set_resources	  = amdk8_domain_set_resources,
	.enable_resources = NULL,
	.init		  = NULL,
	.scan_bus	  = amdk8_domain_scan_bus,
};

static void add_more_links(struct device *dev, unsigned total_links)
{
	struct bus *link, *last = NULL;
	int link_num = -1;

	for (link = dev->link_list; link; link = link->next) {
		if (link_num < link->link_num)
			link_num = link->link_num;
		last = link;
	}

	if (last) {
		int links = total_links - (link_num + 1);
		if (links > 0) {
			link = malloc(links*sizeof(*link));
			if (!link)
				die("Couldn't allocate more links!\n");
			memset(link, 0, links*sizeof(*link));
			last->next = link;
		}
	}
	else {
		link = malloc(total_links*sizeof(*link));
		memset(link, 0, total_links*sizeof(*link));
		dev->link_list = link;
	}

	for (link_num = link_num + 1; link_num < total_links; link_num++) {
		link->link_num = link_num;
		link->dev = dev;
		link->next = link + 1;
		last = link;
		link = link->next;
	}
	last->next = NULL;
}

static void remap_bsp_lapic(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct device *cpu;
	u32 bsp_lapic_id = lapicid();

	if (bsp_lapic_id) {
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = 0;
		cpu = find_dev_path(cpu_bus, &cpu_path);
		if (cpu)
			cpu->path.apic.apic_id = bsp_lapic_id;
	}
}

static void cpu_bus_scan(struct device *dev)
{
	struct bus *cpu_bus;
	struct device *dev_mc;
	int bsp_apicid;
	int i,j;
	unsigned nb_cfg_54;
	unsigned siblings;
	int e0_later_single_core;
	int disable_siblings;

	nb_cfg_54 = 0;
	sysconf.enabled_apic_ext_id = 0;
	sysconf.lift_bsp_apicid = 0;
	siblings = 0;

	/* Find the bootstrap processors apicid */
	bsp_apicid = lapicid();
	sysconf.apicid_offset = bsp_apicid;

	disable_siblings = !CONFIG_LOGICAL_CPUS;
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	get_option(&disable_siblings, "multi_core");
#endif

	// for pre_e0, nb_cfg_54 can not be set, (when you read it still is 0)
	// How can I get the nb_cfg_54 of every node's nb_cfg_54 in bsp???
	// and differ d0 and e0 single core
	nb_cfg_54 = read_nb_cfg_54();

	dev_mc = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	if (!dev_mc) {
		die("0:18.0 not found?");
	}

	sysconf.nodes = ((pci_read_config32(dev_mc, 0x60)>>4) & 7) + 1;


	if (pci_read_config32(dev_mc, 0x68) & (HTTC_APIC_EXT_ID|HTTC_APIC_EXT_BRD_CST))
	{
		sysconf.enabled_apic_ext_id = 1;
		if (bsp_apicid == 0) {
			/* bsp apic id is not changed */
			sysconf.apicid_offset = CONFIG_APIC_ID_OFFSET;
		} else
		{
			sysconf.lift_bsp_apicid = 1;
		}

	}

	/* Find which cpus are present */
	cpu_bus = dev->link_list;

	/* Always use the devicetree node with lapic_id 0 for BSP. */
	remap_bsp_lapic(cpu_bus);

	for (i = 0; i < sysconf.nodes; i++) {
		struct device *cpu_dev;

		/* Find the cpu's pci device */
		cpu_dev = dev_find_slot(0, PCI_DEVFN(0x18 + i, 3));
		if (!cpu_dev) {
			/* If I am probing things in a weird order
			 * ensure all of the cpu's pci devices are found.
			 */
			int local_j;
			struct device *dev_f0;
			for (local_j = 0; local_j <= 3; local_j++) {
				cpu_dev = pci_probe_dev(NULL, dev_mc->bus,
					PCI_DEVFN(0x18 + i, local_j));
			}
			/* Ok, We need to set the links for that device.
			 * otherwise the device under it will not be scanned
			 */
			dev_f0 = dev_find_slot(0, PCI_DEVFN(0x18+i,0));
			if (dev_f0) {
				add_more_links(dev_f0, 3);
			}
		}

		e0_later_single_core = 0;
		int enable_node = cpu_dev && cpu_dev->enabled;
		if (enable_node) {
			j = pci_read_config32(cpu_dev, 0xe8);
			j = (j >> 12) & 3; // dev is func 3
			printk(BIOS_DEBUG, "  %s siblings=%d\n", dev_path(cpu_dev), j);

			if (nb_cfg_54) {
				// For e0 single core if nb_cfg_54 is set, apicid will be 0, 2, 4....
				//  ----> you can mixed single core e0 and dual core e0 at any sequence
				// That is the typical case

				if (j == 0) {
				       #if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
		 		       	e0_later_single_core = is_e0_later_in_bsp(i);  // single core
				       #else
				       	e0_later_single_core = is_cpu_f0_in_bsp(i);  // We can read cpuid(1) from Func3
				       #endif
				} else {
				       e0_later_single_core = 0;
	       			}
				if (e0_later_single_core) {
					printk(BIOS_DEBUG, "\tFound Rev E or Rev F later single core\n");

					j = 1;
				}

				if (siblings > j) {
				}
				else {
					siblings = j;
				}
			} else {
				siblings = j;
  			}
		}

		u32 jj;
		if (e0_later_single_core || disable_siblings) {
			jj = 0;
		} else
		{
			jj = siblings;
		}

		for (j = 0; j <= jj; j++) {
			u32 apic_id = i * (nb_cfg_54?(siblings+1):1) + j * (nb_cfg_54?1:8);
			if (sysconf.enabled_apic_ext_id) {
				if (apic_id != 0 || sysconf.lift_bsp_apicid) {
					apic_id += sysconf.apicid_offset;
				}
			}

			struct device *cpu = add_cpu_device(cpu_bus, apic_id,
					     enable_node);
			if (cpu)
				amd_cpu_topology(cpu, i, j);
		} //j
	}
}

static void cpu_bus_init(struct device *dev)
{
#if IS_ENABLED(CONFIG_WAIT_BEFORE_CPUS_INIT)
	cpus_ready_for_init();
#endif
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = DEVICE_NOOP,
	.set_resources	  = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init		  = cpu_bus_init,
	.scan_bus	  = cpu_bus_scan,
};

static void root_complex_enable_dev(struct device *dev)
{
	static int done = 0;

	/* Do not delay UMA setup, as a device on the PCI bus may evaluate
	   the global uma_memory variables already in its enable function. */
	if (!done) {
		setup_bsp_ramtop();
		setup_uma_memory();
		done = 1;
	}

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_amd_amdk8_root_complex_ops = {
	CHIP_NAME("AMD K8 Root Complex")
	.enable_dev = root_complex_enable_dev,
};
