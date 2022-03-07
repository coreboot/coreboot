/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <lib.h>
#include <cpu/cpu.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <northbridge/amd/nb_common.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <sb_cimx.h>

#define FX_DEVS 1

static struct device *__f0_dev[FX_DEVS];
static struct device *__f1_dev[FX_DEVS];
static struct device *__f2_dev[FX_DEVS];
static struct device *__f4_dev[FX_DEVS];
static unsigned int fx_devs = 0;

static u32 get_io_addr_index(u32 nodeid, u32 linkn)
{
	return 0;
}

static u32 get_mmio_addr_index(u32 nodeid, u32 linkn)
{
	return 0;
}

static void set_io_addr_reg(struct device *dev, u32 nodeid, u32 linkn, u32 reg,
			    u32 io_min, u32 io_max)
{

	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid & 0xf) | ((nodeid & 0x30) << (8 - 4)) | (linkn << 4) |
		  ((io_max & 0xf0) << (12 - 4)); //limit
	pci_write_config32(__f1_dev[0], reg+4, tempreg);

	tempreg = 3 | ((io_min & 0xf0) << (12 - 4)); //base :ISA and VGA ?
	pci_write_config32(__f1_dev[0], reg, tempreg);
}

static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index,
			      u32 mmio_min, u32 mmio_max, u32 nodes)
{

	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid & 0xf) | (linkn << 4) | (mmio_max & 0xffffff00);
	pci_write_config32(__f1_dev[0], reg + 4, tempreg);
	tempreg = 3 | (nodeid & 0x30) | (mmio_min & 0xffffff00);
	pci_write_config32(__f1_dev[0], reg, tempreg);
}

static struct device *get_node_pci(u32 nodeid, u32 fn)
{
	return pcidev_on_root(DEV_CDB + nodeid, fn);
}

static void get_fx_devs(void)
{
	int i;
	for (i = 0; i < FX_DEVS; i++) {
		__f0_dev[i] = get_node_pci(i, 0);
		__f1_dev[i] = get_node_pci(i, 1);
		__f2_dev[i] = get_node_pci(i, 2);
		__f4_dev[i] = get_node_pci(i, 4);
		if (__f0_dev[i] != NULL && __f1_dev[i] != NULL)
			fx_devs = i + 1;
	}
	if (__f1_dev[0] == NULL || __f0_dev[0] == NULL || fx_devs == 0) {
		die("Cannot find 0:0x18.[0|1]\n");
	}
}

static u32 f1_read_config32(unsigned int reg)
{
	if (fx_devs == 0)
		get_fx_devs();
	return pci_read_config32(__f1_dev[0], reg);
}

static void f1_write_config32(unsigned int reg, u32 value)
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

static int get_dram_base_limit(u32 nodeid, resource_t *basek, resource_t *limitk)
{
	u32 temp;

	if (fx_devs == 0)
		get_fx_devs();


	temp = pci_read_config32(__f1_dev[nodeid], 0x40 + (nodeid << 3)); //[39:24] at [31:16]
	if (!(temp & 1))
		return 0; // this memory range is not enabled
	/*
	 * BKDG: {DramBase[35:24], 00_0000h} <= address[35:0] so shift left by 8 bits
	 * for physical address and the convert to KiB by shifting 10 bits left
	 */
	*basek = ((temp & 0x0fff0000)) >> (10 - 8);
	/*
	 * BKDG address[35:0] <= {DramLimit[35:24], FF_FFFFh} converted as above but
	 * ORed with 0xffff to get real limit before shifting.
	 */
	temp = pci_read_config32(__f1_dev[nodeid], 0x44 + (nodeid << 3)); //[39:24] at [31:16]
	*limitk = ((temp & 0x0fff0000) | 0xffff) >> (10 - 8);
	*limitk += 1; // round up last byte

	return 1;
}

static u32 amdfam14_nodeid(struct device *dev)
{
	return (dev->path.pci.devfn >> 3) - DEV_CDB;
}

static void northbridge_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Northbridge init\n");
}

static void set_vga_enable_reg(u32 nodeid, u32 linkn)
{
	u32 val;

	val = 1 | (nodeid << 4) | (linkn << 12);
	/* it will routing (1)mmio  0xa0000:0xbffff (2) io 0x3b0:0x3bb,
	   0x3c0:0x3df */
	f1_write_config32(0xf4, val);

}

static int reg_useable(unsigned int reg, struct device *goal_dev,
		       unsigned int goal_nodeid, unsigned int goal_link)
{
	struct resource *res;
	unsigned int nodeid, link = 0;
	int result;
	res = 0;
	for (nodeid = 0; !res && (nodeid < fx_devs); nodeid++) {
		struct device *dev;
		dev = __f0_dev[nodeid];
		if (!dev)
			continue;
		for (link = 0; !res && (link < 8); link++) {
			res = probe_resource(dev, IOINDEX(0x1000 + reg, link));
		}
	}
	result = 2;
	if (res) {
		result = 0;
		if ((goal_link == (link - 1)) &&
		    (goal_nodeid == (nodeid - 1)) && (res->flags <= 1)) {
			result = 1;
		}
	}
	return result;
}

static struct resource *amdfam14_find_iopair(struct device *dev,
			unsigned int nodeid, unsigned int link)
{
	struct resource *resource;
	u32 result, reg;
	resource = 0;
	reg = 0;
	result = reg_useable(0xc0, dev, nodeid, link);
	if (result >= 1) {
		/* I have been allocated this one */
		reg = 0xc0;
	}
	/* Ext conf space */
	if (!reg) {
		/* Because of Extend conf space, we will never run out of reg,
		 * but we need one index to differ them. So,same node and same
		 *  link can have multi range
		 */
		u32 index = get_io_addr_index(nodeid, link);
		reg = 0x110 + (index << 24) + (4 << 20);	// index could be 0, 255
	}

	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));

	return resource;
}

static struct resource *amdfam14_find_mempair(struct device *dev, u32 nodeid,
					      u32 link)
{
	struct resource *resource;
	u32 free_reg, reg;
	resource = 0;
	free_reg = 0;
	for (reg = 0x80; reg <= 0xb8; reg += 0x8) {
		int result;
		result = reg_useable(reg, dev, nodeid, link);
		if (result == 1) {
			/* I have been allocated this one */
			break;
		} else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xb8) {
		reg = free_reg;
	}
	/* Ext conf space */
	if (!reg) {
		/* Because of Extend conf space, we will never run out of reg,
		 * but we need one index to differ them. So,same node and same
		 *  link can have multi range
		 */
		u32 index = get_mmio_addr_index(nodeid, link);
		reg = 0x110 + (index << 24) + (6 << 20);	// index could be 0, 63

	}
	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));
	return resource;
}

static void amdfam14_link_read_bases(struct device *dev, u32 nodeid, u32 link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource = amdfam14_find_iopair(dev, nodeid, link);
	if (resource) {
		u32 align;
		align = log2(HT_IO_HOST_ALIGN);
		resource->base = 0;
		resource->size = 0;
		resource->align = align;
		resource->gran = align;
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = amdfam14_find_mempair(dev, nodeid, link);
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
	resource = amdfam14_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
	}
}

static u32 my_find_pci_tolm(struct bus *bus, u32 tolm)
{
	struct resource *min;
	unsigned long mask_match = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
	min = 0;
	search_bus_resources(bus, mask_match, mask_match, tolm_test,
			     &min);
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0

struct hw_mem_hole_info {
	unsigned int hole_startk;
	int node_id;
};

static struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
	struct hw_mem_hole_info mem_hole;

	mem_hole.hole_startk = CONFIG_HW_MEM_HOLE_SIZEK;
	mem_hole.node_id = -1;

	resource_t basek, limitk;
	u32 hole;

	if (get_dram_base_limit(0, &basek, &limitk)) {
		hole = pci_read_config32(__f1_dev[0], 0xf0);
		if (hole & 1) {	// we find the hole
			mem_hole.hole_startk = (hole & (0xff << 24)) >> 10;
			mem_hole.node_id = 0;	// record the node No with hole
		}
	}
	return mem_hole;
}
#endif

static void nb_read_resources(struct device *dev)
{
	u32 nodeid;
	struct bus *link;

	printk(BIOS_DEBUG, "\nFam14h - %s\n", __func__);

	nodeid = amdfam14_nodeid(dev);
	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			amdfam14_link_read_bases(dev, nodeid, link->link_num);
		}
	}

	/*
	 * This MMCONF resource must be reserved in the PCI domain.
	 * It is not honored by the coreboot resource allocator if it is in
	 * the CPU_CLUSTER.
	 */
	mmconf_resource(dev, MMIO_CONF_BASE);
}

static void set_resource(struct device *dev, struct resource *resource,
			 u32 nodeid)
{
	resource_t rbase, rend;
	unsigned int reg, link_num;
	char buf[50];

	printk(BIOS_DEBUG, "\nFam14h - %s\n", __func__);

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
	if ((resource->index & 0xffff) < 0x1000) {
		return;
	}
	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend = resource_end(resource);

	/* Get the register and link */
	reg = resource->index & 0xfff;	// 4k
	link_num = IOINDEX_LINK(resource->index);

	if (resource->flags & IORESOURCE_IO) {
		set_io_addr_reg(dev, nodeid, link_num, reg, rbase >> 8,
				rend >> 8);
	} else if (resource->flags & IORESOURCE_MEM) {
		set_mmio_addr_reg(nodeid, link_num, reg, (resource->index >> 24),
				rbase >> 8, rend >> 8, 1);	// [39:8]
	}
	resource->flags |= IORESOURCE_STORED;
	snprintf(buf, sizeof(buf), " <node %x link %x>", nodeid, link_num);
	report_resource_stored(dev, resource, buf);
}

#if CONFIG(CONSOLE_VGA_MULTI)
extern struct device *vga_pri;	// the primary vga device, defined in device.c
#endif

static void create_vga_resource(struct device *dev, unsigned int nodeid)
{
	struct bus *link;

	printk(BIOS_DEBUG, "\nFam14h - %s\n", __func__);

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list; link; link = link->next) {
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if CONFIG(CONSOLE_VGA_MULTI)
			printk(BIOS_DEBUG,
				"VGA: vga_pri bus num = %d bus range [%d,%d]\n",
				 vga_pri->bus->secondary, link->secondary,
				 link->subordinate);
			/* We need to make sure the vga_pri is under the link */
			if ((vga_pri->bus->secondary >= link->secondary) &&
			    (vga_pri->bus->secondary <= link->subordinate))
#endif
				break;
		}
	}

	/* no VGA card installed */
	if (link == NULL)
		return;

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n",
		dev_path(dev), nodeid, link->link_num);
	set_vga_enable_reg(nodeid, link->link_num);
}

static void nb_set_resources(struct device *dev)
{
	unsigned int nodeid;
	struct bus *bus;
	struct resource *res;

	printk(BIOS_DEBUG, "\nFam14h - %s\n", __func__);

	/* Find the nodeid */
	nodeid = amdfam14_nodeid(dev);

	create_vga_resource(dev, nodeid);

	/* Set each resource we have found */
	for (res = dev->resource_list; res; res = res->next) {
		set_resource(dev, res, nodeid);
	}

	for (bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children) {
			assign_resources(bus);
		}
	}
}

/* Domain/Root Complex related code */

static void domain_read_resources(struct device *dev)
{
	unsigned int reg;

	printk(BIOS_DEBUG, "\nFam14h - %s\n", __func__);

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for (reg = 0x80; reg <= 0xc0; reg += 0x08) {
		u32 base, limit;
		base = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned int nodeid, reg_link;
			struct device *reg_dev;
			if (reg < 0xc0) {	// mmio
				nodeid = (limit & 0xf) + (base & 0x30);
			} else {	// io
				nodeid = (limit & 0xf) + ((base >> 4) & 0x30);
			}
			reg_link = (limit >> 4) & 7;
			reg_dev = __f0_dev[nodeid];
			if (reg_dev) {
				/* Reserve the resource  */
				struct resource *res;
				res =
				    new_resource(reg_dev,
						 IOINDEX(0x1000 + reg,
							 reg_link));
				if (res) {
					res->flags = 1;
				}
			}
		}
	}
	/* FIXME: do we need to check extend conf space?
	   I don't believe that much preset value */

	pci_domain_read_resources(dev);
}

static void domain_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "\nFam14h - %s\n", __func__);
	printk(BIOS_DEBUG, "  amsr - incoming dev = %p\n", dev);

	unsigned long mmio_basek;
	u32 pci_tolm;
	int idx;
	struct bus *link;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
	u32 reset_memhole = 1;
#endif

	pci_tolm = 0xffffffffUL;
	for (link = dev->link_list; link; link = link->next) {
		pci_tolm = my_find_pci_tolm(link, pci_tolm);
	}

	// FIXME handle interleaved nodes. If you fix this here, please fix
	// amdk8, too.
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) - 1);

	// FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M
	// MMIO hole. If you fix this here, please fix amdk8, too.
	/* Round the mmio hole to 64M */
	mmio_basek &= ~((64 * 1024) - 1);

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
/* if the hw mem hole is already set in raminit stage, here we will compare
 * mmio_basek and hole_basek. if mmio_basek is bigger that hole_basek and will
 * use hole_basek as mmio_basek and we don't need to reset hole.
 * otherwise We reset the hole to the mmio_basek
 */

	mem_hole = get_hw_mem_hole_info();

	// Use hole_basek as mmio_basek, and we don't need to reset hole anymore
	if ((mem_hole.node_id != -1) && (mmio_basek > mem_hole.hole_startk)) {
		mmio_basek = mem_hole.hole_startk;
		reset_memhole = 0;
	}
#endif

	idx = 0x10;
	resource_t basek, limitk, sizek;	// 4 1T

	if (get_dram_base_limit(0, &basek, &limitk)) {
		sizek = limitk - basek;

		printk(BIOS_DEBUG, "adsr: basek = %llx, limitk = %llx, sizek = %llx.\n",
				   basek, limitk, sizek);

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < 640) && (sizek > 768)) {
			printk(BIOS_DEBUG,"adsr - 0xa0000 to 0xbffff resource.\n");
			ram_resource(dev, (idx | 0), basek, 640 - basek);
			idx += 0x10;
			basek = 768;
			sizek = limitk - 768;
		}

		printk(BIOS_DEBUG,
			"adsr: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
			 mmio_basek, basek, limitk);

		/* split the region to accommodate pci memory space */
		if ((basek < 4 * 1024 * 1024) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned int pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek > 0) {
					ram_resource(dev, idx, basek,
						     pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
				}
				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4 * 1024 * 1024) {
				sizek = 0;
			} else {
				basek = 4 * 1024 * 1024;
				sizek -= (4 * 1024 * 1024 - mmio_basek);
			}
		}

		ram_resource(dev, (idx | 0), basek, sizek);
		idx += 0x10;
		printk(BIOS_DEBUG,
			"%d: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n", 0,
			 mmio_basek, basek, limitk);
	}
	printk(BIOS_DEBUG, "  adsr - mmio_basek = %lx.\n", mmio_basek);

	add_uma_resource_below_tolm(dev, 7);

	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			assign_resources(link);
		}
	}
	printk(BIOS_DEBUG, "  adsr - leaving this lovely routine.\n");
}

static const char *domain_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	return NULL;
}

/* Bus related code */

static void cpu_bus_scan(struct device *dev)
{
	struct bus *cpu_bus = dev->link_list;
	struct device *cpu;
	int apic_id, cores_found;

	/* There is only one node for fam14, but there may be multiple cores. */
	cpu = pcidev_on_root(0x18, 0);
	if (!cpu)
		printk(BIOS_ERR, "%02x:%02x.0 not found", 0, 0x18);

	cores_found = (pci_read_config32(pcidev_on_root(0x18, 0x3),
					0xe8) >> 12) & 3;
	printk(BIOS_DEBUG, "  AP siblings=%d\n", cores_found);

	for (apic_id = 0; apic_id <= cores_found; apic_id++) {
		cpu = add_cpu_device(cpu_bus, apic_id, 1);
		if (cpu)
			amd_cpu_topology(cpu, 0, apic_id);
	}
}

static void cpu_bus_init(struct device *dev)
{
	initialize_cpus(dev->link_list);
}

/* North Bridge Structures */

static void northbridge_fill_ssdt_generator(const struct device *device)
{
	msr_t msr;
	char pscope[] = "\\_SB.PCI0";

	acpigen_write_scope(pscope);
	msr = rdmsr(TOP_MEM);
	acpigen_write_name_dword("TOM1", msr.lo);
	msr = rdmsr(TOP_MEM2);
	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", (msr.hi << 12) | msr.lo >> 20);
	acpigen_pop_len();
}

static unsigned long acpi_fill_hest(acpi_hest_t *hest)
{
	void *addr, *current;

	/* Skip the HEST header. */
	current = (void *)(hest + 1);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_MCE);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 0,
				addr + 2, *(UINT16 *)addr - 2);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_CMC);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 1,
				addr + 2, *(UINT16 *)addr - 2);

	return (unsigned long)current;
}

static void patch_ssdt_processor_scope(acpi_header_t *ssdt)
{
	unsigned int len = ssdt->length - sizeof(acpi_header_t);
	unsigned int i;

	for (i = sizeof(acpi_header_t); i < len; i++) {
		/* Search for _PR_ scope and replace it with _SB_ */
		if (*(uint32_t *)((unsigned long)ssdt + i) == 0x5f52505f)
			*(uint32_t *)((unsigned long)ssdt + i) = 0x5f42535f;
	}
	/* Recalculate checksum */
	ssdt->checksum = 0;
	ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);
}

static unsigned long agesa_write_acpi_tables(const struct device *device,
					     unsigned long current,
					     acpi_rsdp_t *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;
	acpi_header_t *ssdt;
	acpi_header_t *alib;
	acpi_hest_t *hest;

	/* HEST */
	current = ALIGN(current, 8);
	hest = (acpi_hest_t *)current;
	acpi_write_hest((void *)current, acpi_fill_hest);
	acpi_add_table(rsdp, (void *)current);
	current += ((acpi_header_t *)current)->length;

	/* SRAT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:  * SRAT at %lx\n", current);
	srat = (acpi_srat_t *) agesawrapper_getlateinitptr (PICK_SRAT);
	if (srat != NULL) {
		memcpy((void *)current, srat, srat->header.length);
		srat = (acpi_srat_t *) current;
		current += srat->header.length;
		acpi_add_table(rsdp, srat);
	}
	else {
		printk(BIOS_DEBUG, "  AGESA SRAT table NULL. Skipping.\n");
	}

	/* SLIT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:  * SLIT at %lx\n", current);
	slit = (acpi_slit_t *) agesawrapper_getlateinitptr (PICK_SLIT);
	if (slit != NULL) {
		memcpy((void *)current, slit, slit->header.length);
		slit = (acpi_slit_t *) current;
		current += slit->header.length;
		acpi_add_table(rsdp, slit);
	}
	else {
		printk(BIOS_DEBUG, "  AGESA SLIT table NULL. Skipping.\n");
	}

	/* SSDT */
	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:  * AGESA ALIB SSDT at %lx\n", current);
	alib = (acpi_header_t *)agesawrapper_getlateinitptr (PICK_ALIB);
	if (alib != NULL) {
		memcpy((void *)current, alib, alib->length);
		alib = (acpi_header_t *) current;
		current += alib->length;
		acpi_add_table(rsdp, (void *)alib);
	} else {
		printk(BIOS_DEBUG, "	AGESA ALIB SSDT table NULL. Skipping.\n");
	}

	/* The DSDT needs additional work for the AGESA SSDT Pstate table */
	/* Keep the comment for a while. */
	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:  * AGESA SSDT Pstate at %lx\n", current);
	ssdt = (acpi_header_t *)agesawrapper_getlateinitptr (PICK_PSTATE);
	if (ssdt != NULL) {
		hexdump(ssdt, ssdt->length);
		patch_ssdt_processor_scope(ssdt);
		hexdump(ssdt, ssdt->length);
		memcpy((void *)current, ssdt, ssdt->length);
		ssdt = (acpi_header_t *) current;
		current += ssdt->length;
		acpi_add_table(rsdp,ssdt);
	} else {
		printk(BIOS_DEBUG, "  AGESA SSDT Pstate table NULL. Skipping.\n");
	}

	return current;
}

static struct device_operations northbridge_operations = {
	.read_resources = nb_read_resources,
	.set_resources = nb_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.acpi_fill_ssdt = northbridge_fill_ssdt_generator,
	.write_acpi_tables = agesa_write_acpi_tables,
	.init = northbridge_init,
	.enable = 0,.ops_pci = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VID_AMD,
	.device = 0x1510,
};

struct chip_operations northbridge_amd_agesa_family14_ops = {
	CHIP_NAME("AMD Family 14h Northbridge")
	.enable_dev = 0,
};

/* Root Complex Structures */

static struct device_operations pci_domain_ops = {
	.read_resources = domain_read_resources,
	.set_resources = domain_set_resources,
	.scan_bus = pci_domain_scan_bus,
	.acpi_name = domain_acpi_name,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = cpu_bus_init,
	.scan_bus = cpu_bus_scan,
};

static void root_complex_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_amd_agesa_family14_root_complex_ops = {
	CHIP_NAME("AMD Family 14h Root Complex")
	.enable_dev = root_complex_enable_dev,
};

/********************************************************************
* Change the vendor / device IDs to match the generic VBIOS header.
********************************************************************/
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev = vendev;

	switch (vendev) {
	case 0x10029809:
	case 0x10029808:
	case 0x10029807:
	case 0x10029806:
	case 0x10029805:
	case 0x10029804:
	case 0x10029803:
		new_vendev = 0x10029802;
		break;
	}

	return new_vendev;
}
