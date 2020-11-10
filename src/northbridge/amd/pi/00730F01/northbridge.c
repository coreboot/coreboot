/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <acpi/acpi_ivrs.h>
#include <arch/ioapic.h>
#include <types.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdlib.h>
#include <lib.h>
#include <cpu/cpu.h>
#include <Porting.h>
#include <AGESA.h>
#include <Topology.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <acpi/acpigen.h>
#include <northbridge/amd/nb_common.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <southbridge/amd/pi/hudson/pci_devs.h>

#define MAX_NODE_NUMS MAX_NODES
#define PCIE_CAP_AER		BIT(5)
#define PCIE_CAP_ACS		BIT(6)

typedef struct dram_base_mask {
	u32 base; //[47:27] at [28:8]
	u32 mask; //[47:27] at [28:8] and enable at bit 0
} dram_base_mask_t;

static unsigned int node_nums;
static unsigned int sblink;
static struct device *__f0_dev[MAX_NODE_NUMS];
static struct device *__f1_dev[MAX_NODE_NUMS];
static struct device *__f2_dev[MAX_NODE_NUMS];
static struct device *__f4_dev[MAX_NODE_NUMS];
static unsigned int fx_devs = 0;

static dram_base_mask_t get_dram_base_mask(u32 nodeid)
{
	struct device *dev;
	dram_base_mask_t d;
	dev = __f1_dev[0];
	u32 temp;
	temp = pci_read_config32(dev, 0x44 + (nodeid << 3)); //[39:24] at [31:16]
	d.mask = ((temp & 0xfff80000)>>(8+3)); // mask out  DramMask [26:24] too
	temp = pci_read_config32(dev, 0x144 + (nodeid <<3)) & 0xff; //[47:40] at [7:0]
	d.mask |= temp<<21;
	temp = pci_read_config32(dev, 0x40 + (nodeid << 3)); //[39:24] at [31:16]
	d.mask |= (temp & 1); // enable bit
	d.base = ((temp & 0xfff80000)>>(8+3)); // mask out DramBase [26:24) too
	temp = pci_read_config32(dev, 0x140 + (nodeid <<3)) & 0xff; //[47:40] at [7:0]
	d.base |= temp<<21;
	return d;
}

static void set_io_addr_reg(struct device *dev, u32 nodeid, u32 linkn, u32 reg,
			u32 io_min, u32 io_max)
{
	u32 i;
	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid&0xf) | ((nodeid & 0x30)<<(8-4)) | (linkn<<4) | ((io_max&0xf0)<<(12-4)); //limit
	for (i = 0; i < node_nums; i++)
		pci_write_config32(__f1_dev[i], reg+4, tempreg);
	tempreg = 3 /*| (3<<4)*/ | ((io_min&0xf0)<<(12-4));	      //base :ISA and VGA ?
	for (i = 0; i < node_nums; i++)
		pci_write_config32(__f1_dev[i], reg, tempreg);
}

static void set_mmio_addr_reg(u32 nodeid, u32 linkn, u32 reg, u32 index, u32 mmio_min, u32 mmio_max, u32 nodes)
{
	u32 i;
	u32 tempreg;
	/* io range allocation */
	tempreg = (nodeid&0xf) | (linkn<<4) |	 (mmio_max&0xffffff00); //limit
	for (i = 0; i < nodes; i++)
		pci_write_config32(__f1_dev[i], reg+4, tempreg);
	tempreg = 3 | (nodeid & 0x30) | (mmio_min&0xffffff00);
	for (i = 0; i < node_nums; i++)
		pci_write_config32(__f1_dev[i], reg, tempreg);
}

static struct device *get_node_pci(u32 nodeid, u32 fn)
{
	return pcidev_on_root(DEV_CDB + nodeid, fn);
}

static void get_fx_devs(void)
{
	int i;
	for (i = 0; i < MAX_NODE_NUMS; i++) {
		__f0_dev[i] = get_node_pci(i, 0);
		__f1_dev[i] = get_node_pci(i, 1);
		__f2_dev[i] = get_node_pci(i, 2);
		__f4_dev[i] = get_node_pci(i, 4);
		if (__f0_dev[i] != NULL && __f1_dev[i] != NULL)
			fx_devs = i+1;
	}
	if (__f1_dev[0] == NULL || __f0_dev[0] == NULL || fx_devs == 0) {
		die("Cannot find 0:0x18.[0|1]\n");
	}
	printk(BIOS_DEBUG, "fx_devs = 0x%x\n", fx_devs);
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

static u32 amdfam16_nodeid(struct device *dev)
{
	return (dev->path.pci.devfn >> 3) - DEV_CDB;
}

static void set_vga_enable_reg(u32 nodeid, u32 linkn)
{
	u32 val;

	val =  1 | (nodeid<<4) | (linkn<<12);
	/* it will routing
	 * (1)mmio 0xa0000:0xbffff
	 * (2)io   0x3b0:0x3bb, 0x3c0:0x3df
	 */
	f1_write_config32(0xf4, val);

}

/**
 * @return
 *  @retval 2  resource does not exist, usable
 *  @retval 0  resource exists, not usable
 *  @retval 1  resource exist, resource has been allocated before
 */
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
			(goal_nodeid == (nodeid - 1)) &&
			(res->flags <= 1)) {
			result = 1;
		}
	}
	return result;
}

static struct resource *amdfam16_find_iopair(struct device *dev,
		unsigned int nodeid, unsigned int link)
{
	struct resource *resource;
	u32 free_reg, reg;
	resource = 0;
	free_reg = 0;
	for (reg = 0xc0; reg <= 0xd8; reg += 0x8) {
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
		reg = free_reg; // if no free, the free_reg still be 0
	}

	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));

	return resource;
}

static struct resource *amdfam16_find_mempair(struct device *dev, u32 nodeid, u32 link)
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
		}
		else if (result > 1) {
			/* I have a free register pair */
			free_reg = reg;
		}
	}
	if (reg > 0xb8) {
		reg = free_reg;
	}

	resource = new_resource(dev, IOINDEX(0x1000 + reg, link));
	return resource;
}

static void amdfam16_link_read_bases(struct device *dev, u32 nodeid, u32 link)
{
	struct resource *resource;

	/* Initialize the io space constraints on the current bus */
	resource = amdfam16_find_iopair(dev, nodeid, link);
	if (resource) {
		u32 align;
		align = log2(HT_IO_HOST_ALIGN);
		resource->base	= 0;
		resource->size	= 0;
		resource->align = align;
		resource->gran	= align;
		resource->limit = 0xffffUL;
		resource->flags = IORESOURCE_IO | IORESOURCE_BRIDGE;
	}

	/* Initialize the prefetchable memory constraints on the current bus */
	resource = amdfam16_find_mempair(dev, nodeid, link);
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
	resource = amdfam16_find_mempair(dev, nodeid, link);
	if (resource) {
		resource->base = 0;
		resource->size = 0;
		resource->align = log2(HT_MEM_HOST_ALIGN);
		resource->gran = log2(HT_MEM_HOST_ALIGN);
		resource->limit = 0xffffffffffULL;
		resource->flags = IORESOURCE_MEM | IORESOURCE_BRIDGE;
	}

}

static void read_resources(struct device *dev)
{
	u32 nodeid;
	struct bus *link;
	struct resource *res;

	nodeid = amdfam16_nodeid(dev);
	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			amdfam16_link_read_bases(dev, nodeid, link->link_num);
		}
	}

	/*
	 * This MMCONF resource must be reserved in the PCI domain.
	 * It is not honored by the coreboot resource allocator if it is in
	 * the CPU_CLUSTER.
	 */
	mmconf_resource(dev, MMIO_CONF_BASE);

	/* NB IOAPIC2 resource */
	res = new_resource(dev, IO_APIC2_ADDR); /* IOAPIC2 */
	res->base = IO_APIC2_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void set_resource(struct device *dev, struct resource *resource, u32 nodeid)
{
	resource_t rbase, rend;
	unsigned int reg, link_num;
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
	if ((resource->index & 0xffff) < 0x1000) {
		return;
	}
	/* Get the base address */
	rbase = resource->base;

	/* Get the limit (rounded up) */
	rend  = resource_end(resource);

	/* Get the register and link */
	reg  = resource->index & 0xfff; // 4k
	link_num = IOINDEX_LINK(resource->index);

	if (resource->flags & IORESOURCE_IO) {
		set_io_addr_reg(dev, nodeid, link_num, reg, rbase>>8, rend>>8);
	}
	else if (resource->flags & IORESOURCE_MEM) {
		set_mmio_addr_reg(nodeid, link_num, reg, (resource->index >>24), rbase>>8, rend>>8, node_nums); // [39:8]
	}
	resource->flags |= IORESOURCE_STORED;
	snprintf(buf, sizeof(buf), " <node %x link %x>",
			nodeid, link_num);
	report_resource_stored(dev, resource, buf);
}

/**
 * I tried to reuse the resource allocation code in set_resource()
 * but it is too difficult to deal with the resource allocation magic.
 */

static void create_vga_resource(struct device *dev, unsigned int nodeid)
{
	struct bus *link;

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list; link; link = link->next) {
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if CONFIG(MULTIPLE_VGA_ADAPTERS)
			extern struct device *vga_pri; // the primary vga device, defined in device.c
			printk(BIOS_DEBUG, "VGA: vga_pri bus num = %d bus range [%d,%d]\n", vga_pri->bus->secondary,
					link->secondary,link->subordinate);
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

	printk(BIOS_DEBUG, "VGA: %s (aka node %d) link %d has VGA device\n", dev_path(dev), nodeid, sblink);
	set_vga_enable_reg(nodeid, sblink);
}

static void set_resources(struct device *dev)
{
	unsigned int nodeid;
	struct bus *bus;
	struct resource *res;

	/* Find the nodeid */
	nodeid = amdfam16_nodeid(dev);

	create_vga_resource(dev, nodeid); //TODO: do we need this?

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

static void northbridge_init(struct device *dev)
{
	setup_ioapic((u8 *)IO_APIC2_ADDR, CONFIG_MAX_CPUS+1);
}

static unsigned long acpi_fill_hest(acpi_hest_t *hest)
{
	void *addr, *current;

	/* Skip the HEST header. */
	current = (void *)(hest + 1);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_MCE);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 0, (void *)((u32)addr + 2), *(UINT16 *)addr - 2);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_CMC);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 1, (void *)((u32)addr + 2), *(UINT16 *)addr - 2);

	return (unsigned long)current;
}

unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current)
{
	/* 8-byte IVHD structures must be aligned to the 8-byte boundary. */
	current = ALIGN_UP(current, 8);
	ivrs_ivhd_special_t *ivhd_ioapic = (ivrs_ivhd_special_t *)current;

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->reserved = 0x0000;
	ivhd_ioapic->dte_setting = IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
				   IVHD_DTE_SYS_MGT_NO_TRANS | IVHD_DTE_NMI_PASS |
				   IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS;
	ivhd_ioapic->handle = CONFIG_MAX_CPUS; /* FCH IOAPIC ID */
	ivhd_ioapic->source_dev_id = PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC);
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	ivhd_ioapic = (ivrs_ivhd_special_t *)current;

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->reserved = 0x0000;
	ivhd_ioapic->dte_setting = 0x00;
	ivhd_ioapic->handle = CONFIG_MAX_CPUS + 1; /* GNB IOAPIC ID */
	ivhd_ioapic->source_dev_id = PCI_DEVFN(0, 1);
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	return current;
}

static unsigned long ivhd_describe_hpet(unsigned long current)
{
	/* 8-byte IVHD structures must be aligned to the 8-byte boundary. */
	current = ALIGN_UP(current, 8);
	ivrs_ivhd_special_t *ivhd_hpet = (ivrs_ivhd_special_t *)current;

	ivhd_hpet->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_hpet->reserved = 0x0000;
	ivhd_hpet->dte_setting = 0x00;
	ivhd_hpet->handle = 0x00;
	ivhd_hpet->source_dev_id = PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC);
	ivhd_hpet->variety = IVHD_SPECIAL_DEV_HPET;
	current += sizeof(ivrs_ivhd_special_t);

	return current;
}

static unsigned long ivhd_dev_range(unsigned long current, uint16_t start_devid,
				    uint16_t end_devid, uint8_t setting)
{
	/* 4-byte IVHD structures must be aligned to the 4-byte boundary. */
	current = ALIGN_UP(current, 4);
	ivrs_ivhd_generic_t *ivhd_range = (ivrs_ivhd_generic_t *)current;

	/* Create the start range IVHD entry */
	ivhd_range->type = IVHD_DEV_4_BYTE_START_RANGE;
	ivhd_range->dev_id = start_devid;
	ivhd_range->dte_setting = setting;
	current += sizeof(ivrs_ivhd_generic_t);

	/* Create the end range IVHD entry */
	ivhd_range = (ivrs_ivhd_generic_t *)current;
	ivhd_range->type = IVHD_DEV_4_BYTE_END_RANGE;
	ivhd_range->dev_id = end_devid;
	ivhd_range->dte_setting = setting;
	current += sizeof(ivrs_ivhd_generic_t);

	return current;
}

static unsigned long add_ivhd_dev_entry(struct device *parent, struct device *dev,
					unsigned long *current, uint8_t type, uint8_t data)
{
	if (type == IVHD_DEV_4_BYTE_SELECT) {
		/* 4-byte IVHD structures must be aligned to the 4-byte boundary. */
		*current = ALIGN_UP(*current, 4);
		ivrs_ivhd_generic_t *ivhd_entry = (ivrs_ivhd_generic_t *)*current;

		ivhd_entry->type = type;
		ivhd_entry->dev_id = dev->path.pci.devfn | (dev->bus->secondary << 8);
		ivhd_entry->dte_setting = data;
		*current += sizeof(ivrs_ivhd_generic_t);
	} else if (type == IVHD_DEV_8_BYTE_ALIAS_SELECT) {
		/* 8-byte IVHD structures must be aligned to the 8-byte boundary. */
		*current = ALIGN_UP(*current, 8);
		ivrs_ivhd_alias_t *ivhd_entry = (ivrs_ivhd_alias_t *)*current;

		ivhd_entry->type = type;
		ivhd_entry->dev_id = dev->path.pci.devfn | (dev->bus->secondary << 8);
		ivhd_entry->dte_setting = data;
		ivhd_entry->reserved1 = 0;
		ivhd_entry->reserved2 = 0;
		ivhd_entry->source_dev_id = parent->path.pci.devfn |
					    (parent->bus->secondary << 8);
		*current += sizeof(ivrs_ivhd_alias_t);
	}

	return *current;
}

static void ivrs_add_device_or_bridge(struct device *parent, struct device *dev,
				      unsigned long *current, uint16_t *ivhd_length)
{
	unsigned int header_type, is_pcie;
	unsigned long current_backup;

	header_type = dev->hdr_type & 0x7f;
	is_pcie = pci_find_capability(dev, PCI_CAP_ID_PCIE);

	if (((header_type == PCI_HEADER_TYPE_NORMAL) ||
	     (header_type == PCI_HEADER_TYPE_BRIDGE)) && is_pcie) {
		/* Device or Bridge is PCIe */
		current_backup = *current;
		add_ivhd_dev_entry(parent, dev, current, IVHD_DEV_4_BYTE_SELECT, 0x0);
		*ivhd_length += (*current - current_backup);
	} else if ((header_type == PCI_HEADER_TYPE_NORMAL) && !is_pcie) {
		/* Device is legacy PCI or PCI-X */
		current_backup = *current;
		add_ivhd_dev_entry(parent, dev, current, IVHD_DEV_8_BYTE_ALIAS_SELECT, 0x0);
		*ivhd_length += (*current - current_backup);
	}
}

static void add_ivhd_device_entries(struct device *parent, struct device *dev,
				    unsigned int depth, int linknum, int8_t *root_level,
				    unsigned long *current, uint16_t *ivhd_length)
{
	struct device *sibling;
	struct bus *link;

	if (!root_level) {
		root_level = malloc(sizeof(int8_t));
		*root_level = -1;
	}

	if (dev->path.type == DEVICE_PATH_PCI) {

		if ((dev->bus->secondary == 0x0) &&
		    (dev->path.pci.devfn == 0x0))
			*root_level = depth;

		if ((*root_level != -1) && (dev->enabled)) {
			if (depth != *root_level)
				ivrs_add_device_or_bridge(parent, dev, current, ivhd_length);
		}
	}

	for (link = dev->link_list; link; link = link->next)
		for (sibling = link->children; sibling; sibling =
		     sibling->sibling)
			add_ivhd_device_entries(dev, sibling, depth + 1, depth, root_level,
						current, ivhd_length);

	free(root_level);
}

#define IOMMU_MMIO32(x)			(*((volatile uint32_t *)(x)))
#define EFR_SUPPORT			BIT(27)

static unsigned long acpi_fill_ivrs11(unsigned long current, acpi_ivrs_t *ivrs_agesa)
{
	acpi_ivrs_ivhd11_t *ivhd_11;
	unsigned long current_backup;

	/*
	 * These devices should be already found by previous function.
	 * Do not perform NULL checks.
	 */
	struct device *nb_dev = pcidev_on_root(0, 0);
	struct device *iommu_dev = pcidev_on_root(0, 2);

	/*
	 * In order to utilize all features, firmware should expose type 11h
	 * IVHD which supersedes the type 10h.
	 */
	memset((void *)current, 0, sizeof(acpi_ivrs_ivhd11_t));
	ivhd_11 = (acpi_ivrs_ivhd11_t *)current;

	/* Enable EFR */
	ivhd_11->type = IVHD_BLOCK_TYPE_FULL__FIXED;
	/* For type 11h bits 6 and 7 are reserved */
	ivhd_11->flags = ivrs_agesa->ivhd.flags & 0x3f;
	ivhd_11->length = sizeof(struct acpi_ivrs_ivhd_11);
	/* BDF <bus>:00.2 */
	ivhd_11->device_id = 0x02 | (nb_dev->bus->secondary << 8);
	/* PCI Capability block 0x40 (type 0xf, "Secure device") */
	ivhd_11->capability_offset = 0x40;
	ivhd_11->iommu_base_low = ivrs_agesa->ivhd.iommu_base_low;
	ivhd_11->iommu_base_high = ivrs_agesa->ivhd.iommu_base_high;
	ivhd_11->pci_segment_group = 0x0000;
	ivhd_11->iommu_info = ivrs_agesa->ivhd.iommu_info;
	ivhd_11->iommu_attributes.perf_counters =
		(IOMMU_MMIO32(ivhd_11->iommu_base_low + 0x4000) >> 7) & 0xf;
	ivhd_11->iommu_attributes.perf_counter_banks =
		(IOMMU_MMIO32(ivhd_11->iommu_base_low + 0x4000) >> 12) & 0x3f;
	ivhd_11->iommu_attributes.msi_num_ppr =
		(pci_read_config32(iommu_dev, ivhd_11->capability_offset + 0x10) >> 27) & 0x1f;

	if (pci_read_config32(iommu_dev, ivhd_11->capability_offset) & EFR_SUPPORT) {
		ivhd_11->efr_reg_image_low  = IOMMU_MMIO32(ivhd_11->iommu_base_low + 0x30);
		ivhd_11->efr_reg_image_high = IOMMU_MMIO32(ivhd_11->iommu_base_low + 0x34);
	}

	current += sizeof(acpi_ivrs_ivhd11_t);

	/* Now repeat all the device entries from type 10h */
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), PCI_DEVFN(0x1f, 6), 0);
	ivhd_11->length += (current - current_backup);
	add_ivhd_device_entries(NULL, all_devices, 0, -1, NULL, &current, &ivhd_11->length);

	/* Describe HPET */
	current_backup = current;
	current = ivhd_describe_hpet(current);
	ivhd_11->length += (current - current_backup);

	/* Describe IOAPICs */
	current_backup = current;
	current = acpi_fill_ivrs_ioapic(ivrs_agesa, current);
	ivhd_11->length += (current - current_backup);

	return current;
}

static unsigned long acpi_fill_ivrs(acpi_ivrs_t *ivrs, unsigned long current)
{
	acpi_ivrs_t *ivrs_agesa;
	unsigned long current_backup;

	struct device *nb_dev = pcidev_on_root(0, 0);
	if (!nb_dev) {
		printk(BIOS_WARNING, "%s: G-series northbridge device not present!\n", __func__);
		printk(BIOS_WARNING, "%s: IVRS table not generated...\n", __func__);

		return (unsigned long)ivrs;
	}

	struct device *iommu_dev = pcidev_on_root(0, 2);

	if (!iommu_dev) {
		printk(BIOS_WARNING, "%s: IOMMU device not found\n", __func__);

		return (unsigned long)ivrs;
	}

	ivrs_agesa = agesawrapper_getlateinitptr(PICK_IVRS);
	if (ivrs_agesa != NULL) {
		ivrs->iv_info = ivrs_agesa->iv_info;
		ivrs->ivhd.type = IVHD_BLOCK_TYPE_LEGACY__FIXED;
		ivrs->ivhd.flags = ivrs_agesa->ivhd.flags;
		ivrs->ivhd.length = sizeof(struct acpi_ivrs_ivhd);
		/* BDF <bus>:00.2 */
		ivrs->ivhd.device_id = 0x02 | (nb_dev->bus->secondary << 8);
		/* PCI Capability block 0x40 (type 0xf, "Secure device") */
		ivrs->ivhd.capability_offset = 0x40;
		ivrs->ivhd.iommu_base_low = ivrs_agesa->ivhd.iommu_base_low;
		ivrs->ivhd.iommu_base_high = ivrs_agesa->ivhd.iommu_base_high;
		ivrs->ivhd.pci_segment_group = 0x0000;
		ivrs->ivhd.iommu_info = ivrs_agesa->ivhd.iommu_info;
		ivrs->ivhd.iommu_feature_info = ivrs_agesa->ivhd.iommu_feature_info;
		/* Enable EFR if supported */
		if (pci_read_config32(iommu_dev, ivrs->ivhd.capability_offset) & EFR_SUPPORT)
			ivrs->iv_info |= IVINFO_EFR_SUPPORTED;
	} else {
		printk(BIOS_WARNING, "%s: AGESA returned NULL IVRS\n", __func__);

		return (unsigned long)ivrs;
	}

	/*
	 * Add all possible PCI devices on bus 0 that can generate transactions
	 * processed by IOMMU. Start with device 00:01.0 since IOMMU does not
	 * translate transactions generated by itself.
	 */
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), PCI_DEVFN(0x1f, 6), 0);
	ivrs->ivhd.length += (current - current_backup);
	add_ivhd_device_entries(NULL, all_devices, 0, -1, NULL, &current, &ivrs->ivhd.length);

	/* Describe HPET */
	current_backup = current;
	current = ivhd_describe_hpet(current);
	ivrs->ivhd.length += (current - current_backup);

	/* Describe IOAPICs */
	current_backup = current;
	current = acpi_fill_ivrs_ioapic(ivrs_agesa, current);
	ivrs->ivhd.length += (current - current_backup);

	/* If EFR is not supported, IVHD type 11h is reserved */
	if (!(ivrs->iv_info & IVINFO_EFR_SUPPORTED))
		return current;

	return acpi_fill_ivrs11(current, ivrs_agesa);
}

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
	acpi_ivrs_t *ivrs;

	/* HEST */
	current = ALIGN(current, 8);
	acpi_write_hest((void *)current, acpi_fill_hest);
	acpi_add_table(rsdp, (void *)current);
	current += ((acpi_header_t *)current)->length;

	/* IVRS */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * IVRS at %lx\n", current);
	ivrs = (acpi_ivrs_t *) current;
	acpi_create_ivrs(ivrs, acpi_fill_ivrs);
	current += ivrs->header.length;
	acpi_add_table(rsdp, ivrs);

	/* SRAT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *) agesawrapper_getlateinitptr (PICK_SRAT);
	if (srat != NULL) {
		memcpy((void *)current, srat, srat->header.length);
		srat = (acpi_srat_t *) current;
		current += srat->header.length;
		acpi_add_table(rsdp, srat);
	} else {
		printk(BIOS_DEBUG, "  AGESA SRAT table NULL. Skipping.\n");
	}

	/* SLIT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *) agesawrapper_getlateinitptr (PICK_SLIT);
	if (slit != NULL) {
		memcpy((void *)current, slit, slit->header.length);
		slit = (acpi_slit_t *) current;
		current += slit->header.length;
		acpi_add_table(rsdp, slit);
	} else {
		printk(BIOS_DEBUG, "  AGESA SLIT table NULL. Skipping.\n");
	}

	/* ALIB */
	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:  * AGESA ALIB SSDT at %lx\n", current);
	alib = (acpi_header_t *)agesawrapper_getlateinitptr (PICK_ALIB);
	if (alib != NULL) {
		memcpy((void *)current, alib, alib->length);
		alib = (acpi_header_t *) current;
		current += alib->length;
		acpi_add_table(rsdp, (void *)alib);
	}
	else {
		printk(BIOS_DEBUG, "	AGESA ALIB SSDT table NULL. Skipping.\n");
	}

	/* this pstate ssdt may cause Blue Screen: Fixed: Keep this comment for a while. */
	/* SSDT */
	current   = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:    * SSDT at %lx\n", current);
	ssdt = (acpi_header_t *)agesawrapper_getlateinitptr (PICK_PSTATE);
	if (ssdt != NULL) {
		patch_ssdt_processor_scope(ssdt);
		memcpy((void *)current, ssdt, ssdt->length);
		ssdt = (acpi_header_t *) current;
		current += ssdt->length;
	}
	else {
		printk(BIOS_DEBUG, "  AGESA PState table NULL. Skipping.\n");
	}
	acpi_add_table(rsdp,ssdt);

	printk(BIOS_DEBUG, "ACPI:    * SSDT for PState at %lx\n", current);
	return current;
}

static struct device_operations northbridge_operations = {
	.read_resources	  = read_resources,
	.set_resources	  = set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = northbridge_init,
	.acpi_fill_ssdt   = northbridge_fill_ssdt_generator,
	.write_acpi_tables = agesa_write_acpi_tables,
};

static const struct pci_driver family16_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_16H_MODEL_303F_NB_HT,
};

static const struct pci_driver family10_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_10H_NB_HT,
};

static void fam16_finalize(void *chip_info)
{
	struct device *dev;
	u32 value;
	dev = pcidev_on_root(0, 0); /* clear IoapicSbFeatureEn */
	pci_write_config32(dev, 0xF8, 0);
	pci_write_config32(dev, 0xFC, 5); /* TODO: move it to dsdt.asl */

	/*
	 * Currently it is impossible to enable ACS with AGESA by setting the
	 * correct bit for AmdInitMid phase. AGESA code path does not call the
	 * right function that enables these functionalities. Disabled ACS
	 * result in multiple PCIe devices to be assigned to the same IOMMU
	 * group. Without IOMMU group separation the devices cannot be passed
	 * through independently.
	 */

	/* Select GPP link core IO Link Strap Control register 0xB0 */
	pci_write_config32(dev, 0xE0, 0x014000B0);
	value = pci_read_config32(dev, 0xE4);

	/* Enable AER (bit 5) and ACS (bit 6 undocumented) */
	value |= PCIE_CAP_AER | PCIE_CAP_ACS;
	pci_write_config32(dev, 0xE4, value);

	/* Select GPP link core Wrapper register 0x00 (undocumented) */
	pci_write_config32(dev, 0xE0, 0x01300000);
	value = pci_read_config32(dev, 0xE4);

	/*
	 * Enable ACS capabilities straps including sub-items. From lspci it
	 * looks like these bits enable: Source Validation and Translation
	 * Blocking
	 */
	value |= (BIT(24) | BIT(25) | BIT(26));
	pci_write_config32(dev, 0xE4, value);

	/* disable No Snoop */
	dev = pcidev_on_root(1, 1);
	if (dev != NULL) {
		value = pci_read_config32(dev, 0x60);
		value &= ~(1 << 11);
		pci_write_config32(dev, 0x60, value);
	}
}

struct chip_operations northbridge_amd_pi_00730F01_ops = {
	CHIP_NAME("AMD FAM16 Northbridge")
	.enable_dev = 0,
	.final = fam16_finalize,
};

static void domain_read_resources(struct device *dev)
{
	unsigned int reg;

	/* Find the already assigned resource pairs */
	get_fx_devs();
	for (reg = 0x80; reg <= 0xd8; reg+= 0x08) {
		u32 base, limit;
		base  = f1_read_config32(reg);
		limit = f1_read_config32(reg + 0x04);
		/* Is this register allocated? */
		if ((base & 3) != 0) {
			unsigned int nodeid, reg_link;
			struct device *reg_dev;
			if (reg < 0xc0) { // mmio
				nodeid = (limit & 0xf) + (base&0x30);
			} else { // io
				nodeid =  (limit & 0xf) + ((base>>4)&0x30);
			}
			reg_link = (limit >> 4) & 7;
			reg_dev = __f0_dev[nodeid];
			if (reg_dev) {
				/* Reserve the resource  */
				struct resource *res;
				res = new_resource(reg_dev, IOINDEX(0x1000 + reg, reg_link));
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

static void domain_enable_resources(struct device *dev)
{
}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
struct hw_mem_hole_info {
	unsigned int hole_startk;
	int node_id;
};
static struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
	struct hw_mem_hole_info mem_hole;
	int i;
	mem_hole.hole_startk = CONFIG_HW_MEM_HOLE_SIZEK;
	mem_hole.node_id = -1;
	for (i = 0; i < node_nums; i++) {
		dram_base_mask_t d;
		u32 hole;
		d = get_dram_base_mask(i);
		if (!(d.mask & 1)) continue; // no memory on this node
		hole = pci_read_config32(__f1_dev[i], 0xf0);
		if (hole & 2) { // we find the hole
			mem_hole.hole_startk = (hole & (0xff<<24)) >> 10;
			mem_hole.node_id = i; // record the node No with hole
			break; // only one hole
		}
	}

	/* We need to double check if there is special set on base reg and limit reg
	 * are not continuous instead of hole, it will find out its hole_startk.
	 */
	if (mem_hole.node_id == -1) {
		resource_t limitk_pri = 0;
		for (i = 0; i < node_nums; i++) {
			dram_base_mask_t d;
			resource_t base_k, limit_k;
			d = get_dram_base_mask(i);
			if (!(d.base & 1)) continue;
			base_k = ((resource_t)(d.base & 0x1fffff00)) <<9;
			if (base_k > 4 *1024 * 1024) break; // don't need to go to check
			if (limitk_pri != base_k) { // we find the hole
				mem_hole.hole_startk = (unsigned int)limitk_pri; // must be below 4G
				mem_hole.node_id = i;
				break; //only one hole
			}
			limit_k = ((resource_t)(((d.mask & ~1) + 0x000FF) & 0x1fffff00)) << 9;
			limitk_pri = limit_k;
		}
	}
	return mem_hole;
}
#endif

static void domain_set_resources(struct device *dev)
{
	unsigned long mmio_basek;
	u32 pci_tolm;
	int i, idx;
	struct bus *link;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
#endif

	pci_tolm = 0xffffffffUL;
	for (link = dev->link_list; link; link = link->next) {
		pci_tolm = find_pci_tolm(link);
	}

	// FIXME handle interleaved nodes. If you fix this here, please fix
	// amdk8, too.
	mmio_basek = pci_tolm >> 10;
	/* Round mmio_basek to something the processor can support */
	mmio_basek &= ~((1 << 6) -1);

	// FIXME improve mtrr.c so we don't use up all of the mtrrs with a 64M
	// MMIO hole. If you fix this here, please fix amdk8, too.
	/* Round the mmio hole to 64M */
	mmio_basek &= ~((64*1024) - 1);

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	/* if the hw mem hole is already set in raminit stage, here we will compare
	 * mmio_basek and hole_basek. if mmio_basek is bigger that hole_basek and will
	 * use hole_basek as mmio_basek and we don't need to reset hole.
	 * otherwise We reset the hole to the mmio_basek
	 */

	mem_hole = get_hw_mem_hole_info();

	// Use hole_basek as mmio_basek, and we don't need to reset hole anymore
	if ((mem_hole.node_id !=  -1) && (mmio_basek > mem_hole.hole_startk)) {
		mmio_basek = mem_hole.hole_startk;
	}
#endif

	idx = 0x10;
	for (i = 0; i < node_nums; i++) {
		dram_base_mask_t d;
		resource_t basek, limitk, sizek; // 4 1T

		d = get_dram_base_mask(i);

		if (!(d.mask & 1)) continue;
		basek = ((resource_t)(d.base & 0x1fffff00)) << 9; // could overflow, we may lost 6 bit here
		limitk = ((resource_t)(((d.mask & ~1) + 0x000FF) & 0x1fffff00)) << 9;

		sizek = limitk - basek;

		/* see if we need a hole from 0xa0000 to 0xbffff */
		if ((basek < ((8*64)+(8*16))) && (sizek > ((8*64)+(16*16)))) {
			ram_resource(dev, (idx | i), basek, ((8*64)+(8*16)) - basek);
			idx += 0x10;
			basek = (8*64)+(16*16);
			sizek = limitk - ((8*64)+(16*16));

		}

		//printk(BIOS_DEBUG, "node %d : mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n", i, mmio_basek, basek, limitk);

		/* split the region to accommodate pci memory space */
		if ((basek < 4*1024*1024) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned int pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek > 0) {
					ram_resource(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
				}
				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4*1024*1024) {
				sizek = 0;
			}
			else {
				uint64_t topmem2 = bsp_topmem2();
				basek = 4*1024*1024;
				sizek = topmem2/1024 - basek;
			}
		}

		ram_resource(dev, (idx | i), basek, sizek);
		idx += 0x10;
		printk(BIOS_DEBUG, "node %d: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
				i, mmio_basek, basek, limitk);
	}

	add_uma_resource_below_tolm(dev, 7);

	for (link = dev->link_list; link; link = link->next) {
		if (link->children) {
			assign_resources(link);
		}
	}
}

static const char *domain_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = domain_set_resources,
	.enable_resources = domain_enable_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name        = domain_acpi_name,
};

static void sysconf_init(struct device *dev) // first node
{
	sblink = (pci_read_config32(dev, 0x64)>>8) & 7; // don't forget sublink1
	node_nums = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1; //NodeCnt[2:0]
}

static void cpu_bus_scan(struct device *dev)
{
	struct bus *cpu_bus;
	struct device *dev_mc;
	int i,j;
	int coreid_bits;
	int core_max = 0;
	unsigned int ApicIdCoreIdSize;
	unsigned int core_nums;
	int siblings = 0;
	unsigned int family;
	u32 modules = 0;
	int ioapic_count = 0;

	/* For binaryPI there is no multiprocessor configuration, the number of
	 * modules will always be 1. */
	modules = 1;
	ioapic_count = CONFIG_NUM_OF_IOAPICS;

	dev_mc = pcidev_on_root(DEV_CDB, 0);
	if (!dev_mc) {
		printk(BIOS_ERR, "0:%02x.0 not found", DEV_CDB);
		die("");
	}
	sysconf_init(dev_mc);

	/* Get Max Number of cores(MNC) */
	coreid_bits = (cpuid_ecx(0x80000008) & 0x0000F000) >> 12;
	core_max = 1 << (coreid_bits & 0x000F); //mnc

	ApicIdCoreIdSize = ((cpuid_ecx(0x80000008)>>12) & 0xF);
	if (ApicIdCoreIdSize) {
		core_nums = (1 << ApicIdCoreIdSize) - 1;
	} else {
		core_nums = 3; //quad core
	}

	/* Find which cpus are present */
	cpu_bus = dev->link_list;
	for (i = 0; i < node_nums; i++) {
		struct device *cdb_dev;
		unsigned int devn;
		struct bus *pbus;

		devn = DEV_CDB + i;
		pbus = dev_mc->bus;

		/* Find the cpu's pci device */
		cdb_dev = pcidev_on_root(devn, 0);
		if (!cdb_dev) {
			/* If I am probing things in a weird order
			 * ensure all of the cpu's pci devices are found.
			 */
			int fn;
			for (fn = 0; fn <= 5; fn++) { //FBDIMM?
				cdb_dev = pci_probe_dev(NULL, pbus,
							PCI_DEVFN(devn, fn));
			}
			cdb_dev = pcidev_on_root(devn, 0);
		} else {
			/* Ok, We need to set the links for that device.
			 * otherwise the device under it will not be scanned
			 */

			add_more_links(cdb_dev, 4);
		}

		family = cpuid_eax(1);
		family = (family >> 20) & 0xFF;
		if (family == 1) { //f10
			u32 dword;
			cdb_dev = pcidev_on_root(devn, 3);
			dword = pci_read_config32(cdb_dev, 0xe8);
			siblings = ((dword & BIT15) >> 13) | ((dword & (BIT13 | BIT12)) >> 12);
		} else if (family == 7) {//f16
			cdb_dev = pcidev_on_root(devn, 5);
			if (cdb_dev && cdb_dev->enabled) {
				siblings = pci_read_config32(cdb_dev, 0x84);
				siblings &= 0xFF;
			}
		} else {
			siblings = 0; //default one core
		}
		int enable_node = cdb_dev && cdb_dev->enabled;
		printk(BIOS_SPEW, "%s family%xh, core_max = 0x%x, core_nums = 0x%x, siblings = 0x%x\n",
				dev_path(cdb_dev), 0x0f + family, core_max, core_nums, siblings);

		for (j = 0; j <= siblings; j++) {
			u32 lapicid_start = 0;

			/*
			 * APIC ID calculation is tightly coupled with AGESA v5 code.
			 * This calculation MUST match the assignment calculation done
			 * in LocalApicInitializationAtEarly() function.
			 * And reference GetLocalApicIdForCore()
			 *
			 * Apply APIC enumeration rules
			 * For systems with >= 16 APICs, put the IO-APICs at 0..n and
			 * put the local-APICs at m..z
			 *
			 * This is needed because many IO-APIC devices only have 4 bits
			 * for their APIC id and therefore must reside at 0..15
			 */
			if ((node_nums * core_max) + ioapic_count >= 0x10) {
				lapicid_start = (ioapic_count - 1) / core_max;
				lapicid_start = (lapicid_start + 1) * core_max;
				printk(BIOS_SPEW, "lpaicid_start = 0x%x ", lapicid_start);
			}
			u32 apic_id = (lapicid_start * (i/modules + 1)) + ((i % modules) ? (j + (siblings + 1)) : j);
			printk(BIOS_SPEW, "node 0x%x core 0x%x apicid = 0x%x\n",
					i, j, apic_id);

			struct device *cpu = add_cpu_device(cpu_bus, apic_id, enable_node);
			if (cpu)
				amd_cpu_topology(cpu, i, j);
		} //j
	}
}

static void cpu_bus_init(struct device *dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = noop_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = cpu_bus_init,
	.scan_bus	  = cpu_bus_scan,
};

static void root_complex_enable_dev(struct device *dev)
{
	static int done = 0;

	if (!done) {
		setup_bsp_ramtop();
		done = 1;
	}

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_amd_pi_00730F01_root_complex_ops = {
	CHIP_NAME("AMD FAM16 Root Complex")
	.enable_dev = root_complex_enable_dev,
};

/*********************************************************************
 * Change the vendor / device IDs to match the generic VBIOS header. *
 *********************************************************************/
u32 map_oprom_vendev(u32 vendev)
{
	u32 new_vendev;
	new_vendev =
		((0x10029850 <= vendev) && (vendev <= 0x1002986F)) ? 0x10029850 : vendev;

	if (vendev != new_vendev)
		printk(BIOS_NOTICE, "Mapping PCI device %8x to %8x\n", vendev, new_vendev);

	return new_vendev;
}
