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
#include <cpu/x86/mp.h>
#include <Porting.h>
#include <AGESA.h>
#include <Topology.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <acpi/acpigen.h>
#include <northbridge/amd/nb_common.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <southbridge/amd/pi/hudson/pci_devs.h>
#include <amdblocks/cpu.h>

#define MAX_NODE_NUMS MAX_NODES
#define PCIE_CAP_AER		BIT(5)
#define PCIE_CAP_ACS		BIT(6)

static struct device *__f0_dev[MAX_NODE_NUMS];
static struct device *__f1_dev[MAX_NODE_NUMS];
static struct device *__f2_dev[MAX_NODE_NUMS];
static struct device *__f4_dev[MAX_NODE_NUMS];
static unsigned int fx_devs = 0;

static struct device *get_node_pci(u32 nodeid, u32 fn)
{
	return pcidev_on_root(DEV_CDB + nodeid, fn);
}

static struct device *get_mc_dev(void)
{
	return pcidev_on_root(DEV_CDB, 0);
}

static unsigned int get_node_nums(void)
{
	static unsigned int node_nums;

	if (node_nums)
		return node_nums;

	node_nums = ((pci_read_config32(get_mc_dev(), 0x60) >> 4) & 7) + 1; //NodeCnt[2:0]

	return node_nums;
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
			fx_devs = i + 1;
	}
	if (__f1_dev[0] == NULL || __f0_dev[0] == NULL || fx_devs == 0) {
		die("Cannot find 0:0x18.[0|1]\n");
	}
	printk(BIOS_DEBUG, "fx_devs = 0x%x\n", fx_devs);
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
	 * BKDG: {DramBase[39:24], 00_0000h} <= address[39:0] so shift left by 8 bits
	 * for physical address and the convert to KiB by shifting 10 bits left
	 */
	*basek = ((temp & 0xffff0000)) >> (10 - 8);
	/*
	 * BKDG address[39:0] <= {DramLimit[39:24], FF_FFFFh} converted as above but
	 * ORed with 0xffff to get real limit before shifting.
	 */
	temp = pci_read_config32(__f1_dev[nodeid], 0x44 + (nodeid << 3)); //[39:24] at [31:16]
	*limitk = ((temp & 0xffff0000) | 0xffff) >> (10 - 8);
	*limitk += 1; // round up last byte

	return 1;
}

static u32 amdfam16_nodeid(struct device *dev)
{
	return (dev->path.pci.devfn >> 3) - DEV_CDB;
}

static void set_vga_enable_reg(u32 nodeid, u32 linkn)
{
	u32 val;

	val =  1 | (nodeid << 4) | (linkn << 12);
	/* it will routing
	 * (1)mmio 0xa0000:0xbffff
	 * (2)io   0x3b0:0x3bb, 0x3c0:0x3df
	 */
	f1_write_config32(0xf4, val);

}

static void add_fixed_resources(struct device *dev, int index)
{
	/* Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: option ROMs and SeaBIOS (if used)
	 */
	mmio_resource_kb(dev, index++, 0xa0000 >> 10, (0xc0000 - 0xa0000) >> 10);
	reserved_ram_resource_kb(dev, index++, 0xc0000 >> 10, (0x100000 - 0xc0000) >> 10);

	if (fx_devs == 0)
		get_fx_devs();

	/* Check if CC6 save area is enabled (bit 18 CC6SaveEn)  */
	if (pci_read_config32(__f2_dev[0], 0x118) & (1 << 18)) {
		/* Add CC6 DRAM UC resource residing at DRAM Limit of size 16MB as per BKDG */
		resource_t basek, limitk;
		if (!get_dram_base_limit(0, &basek, &limitk))
			return;
		mmio_resource_kb(dev, index++, limitk, 16 * 1024);
	}
}

static void nb_read_resources(struct device *dev)
{
	struct resource *res;

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

	add_fixed_resources(dev, 0);
}

static void create_vga_resource(struct device *dev, unsigned int nodeid)
{
	struct bus *link;
	unsigned int sblink;

	sblink = (pci_read_config32(get_mc_dev(), 0x64) >> 8) & 7; // don't forget sublink1

	/* find out which link the VGA card is connected,
	 * we only deal with the 'first' vga card */
	for (link = dev->link_list; link; link = link->next) {
		if (link->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
#if CONFIG(MULTIPLE_VGA_ADAPTERS)
			extern struct device *vga_pri; // the primary vga device, defined in device.c
			printk(BIOS_DEBUG, "VGA: vga_pri bus num = %d bus range [%d,%d]\n", vga_pri->bus->secondary,
					link->secondary, link->subordinate);
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

static void nb_set_resources(struct device *dev)
{
	unsigned int nodeid;

	/* Find the nodeid */
	nodeid = amdfam16_nodeid(dev);

	create_vga_resource(dev, nodeid); //TODO: do we need this?

	pci_dev_set_resources(dev);
}

static void northbridge_init(struct device *dev)
{
	setup_ioapic((u8 *)IO_APIC2_ADDR, CONFIG_MAX_CPUS + 1);
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
	ivrs = (acpi_ivrs_t *)current;
	acpi_create_ivrs(ivrs, acpi_fill_ivrs);
	current += ivrs->header.length;
	acpi_add_table(rsdp, ivrs);

	/* SRAT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *)agesawrapper_getlateinitptr(PICK_SRAT);
	if (srat != NULL) {
		memcpy((void *)current, srat, srat->header.length);
		srat = (acpi_srat_t *)current;
		current += srat->header.length;
		acpi_add_table(rsdp, srat);
	} else {
		printk(BIOS_DEBUG, "  AGESA SRAT table NULL. Skipping.\n");
	}

	/* SLIT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *)agesawrapper_getlateinitptr(PICK_SLIT);
	if (slit != NULL) {
		memcpy((void *)current, slit, slit->header.length);
		slit = (acpi_slit_t *)current;
		current += slit->header.length;
		acpi_add_table(rsdp, slit);
	} else {
		printk(BIOS_DEBUG, "  AGESA SLIT table NULL. Skipping.\n");
	}

	/* ALIB */
	current = ALIGN(current, 16);
	printk(BIOS_DEBUG, "ACPI:  * AGESA ALIB SSDT at %lx\n", current);
	alib = (acpi_header_t *)agesawrapper_getlateinitptr(PICK_ALIB);
	if (alib != NULL) {
		memcpy((void *)current, alib, alib->length);
		alib = (acpi_header_t *)current;
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
	ssdt = (acpi_header_t *)agesawrapper_getlateinitptr(PICK_PSTATE);
	if (ssdt != NULL) {
		patch_ssdt_processor_scope(ssdt);
		memcpy((void *)current, ssdt, ssdt->length);
		ssdt = (acpi_header_t *)current;
		current += ssdt->length;
	}
	else {
		printk(BIOS_DEBUG, "  AGESA PState table NULL. Skipping.\n");
	}
	acpi_add_table(rsdp, ssdt);

	printk(BIOS_DEBUG, "ACPI:    * SSDT for PState at %lx\n", current);
	return current;
}

static struct device_operations northbridge_operations = {
	.read_resources	  = nb_read_resources,
	.set_resources	  = nb_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = northbridge_init,
	.ops_pci           = &pci_dev_ops_pci,
	.acpi_fill_ssdt   = northbridge_fill_ssdt_generator,
	.write_acpi_tables = agesa_write_acpi_tables,
};

static const struct pci_driver family16_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_16H_MODEL_303F_NB_HT,
};

static const struct pci_driver family10_northbridge __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_10H_NB_HT,
};

static void fam16_finalize(void *chip_info)
{
	struct device *dev;
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

	/* Enable AER (bit 5) and ACS (bit 6 undocumented) */
	pci_or_config32(dev, 0xE4, PCIE_CAP_AER | PCIE_CAP_ACS);

	/* Select GPP link core Wrapper register 0x00 (undocumented) */
	pci_write_config32(dev, 0xE0, 0x01300000);

	/*
	 * Enable ACS capabilities straps including sub-items. From lspci it
	 * looks like these bits enable: Source Validation and Translation
	 * Blocking
	 */
	pci_or_config32(dev, 0xE4, (BIT(24) | BIT(25) | BIT(26)));

	/* disable No Snoop */
	dev = pcidev_on_root(1, 1);
	if (dev != NULL) {
		pci_and_config32(dev, 0x60, ~(1 << 11));
	}
}

struct chip_operations northbridge_amd_pi_00730F01_ops = {
	CHIP_NAME("AMD FAM16 Northbridge")
	.enable_dev = 0,
	.final = fam16_finalize,
};

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
	for (i = 0; i < get_node_nums(); i++) {
		resource_t basek, limitk;
		u32 hole;
		if (!get_dram_base_limit(i, &basek, &limitk))
			continue; // no memory on this node
		hole = pci_read_config32(__f1_dev[i], 0xf0);
		if (hole & 2) { // we find the hole
			mem_hole.hole_startk = (hole & (0xff << 24)) >> 10;
			mem_hole.node_id = i; // record the node No with hole
			break; // only one hole
		}
	}

	/* We need to double check if there is special set on base reg and limit reg
	 * are not continuous instead of hole, it will find out its hole_startk.
	 */
	if (mem_hole.node_id == -1) {
		resource_t limitk_pri = 0;
		for (i = 0; i < get_node_nums(); i++) {
			resource_t base_k, limit_k;
			if (!get_dram_base_limit(i, &base_k, &limit_k))
				continue; // no memory on this node
			if (base_k > 4 * 1024 * 1024) break; // don't need to go to check
			if (limitk_pri != base_k) { // we find the hole
				mem_hole.hole_startk = (unsigned int)limitk_pri; // must be below 4G
				mem_hole.node_id = i;
				break; //only one hole
			}
			limitk_pri = limit_k;
		}
	}
	return mem_hole;
}
#endif

static void domain_read_resources(struct device *dev)
{
	unsigned long mmio_basek;
	int i, idx;
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	struct hw_mem_hole_info mem_hole;
#endif

	pci_domain_read_resources(dev);

	/* TOP_MEM MSR is our boundary between DRAM and MMIO under 4G */
	mmio_basek = amd_topmem() >> 10;

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
	for (i = 0; i < get_node_nums(); i++) {
		resource_t basek, limitk, sizek; // 4 1T

		if (!get_dram_base_limit(i, &basek, &limitk))
			continue; // no memory on this node

		sizek = limitk - basek;

		printk(BIOS_DEBUG, "node %d: basek=%08llx, limitk=%08llx, sizek=%08llx,\n",
				   i, basek, limitk, sizek);

		/* See if we need a hole from 0xa0000 (640K) to 0xfffff (1024K) */
		if (basek < 640 && sizek > 1024) {
			ram_resource_kb(dev, (idx | i), basek, 640 - basek);
			idx += 0x10;
			basek = 1024;
			sizek = limitk - basek;
		}

		printk(BIOS_DEBUG, "node %d: basek=%08llx, limitk=%08llx, sizek=%08llx,\n",
				   i, basek, limitk, sizek);

		/* split the region to accommodate pci memory space */
		if ((basek < 4 * 1024 * 1024) && (limitk > mmio_basek)) {
			if (basek <= mmio_basek) {
				unsigned int pre_sizek;
				pre_sizek = mmio_basek - basek;
				if (pre_sizek > 0) {
					ram_resource_kb(dev, (idx | i), basek, pre_sizek);
					idx += 0x10;
					sizek -= pre_sizek;
				}
				basek = mmio_basek;
			}
			if ((basek + sizek) <= 4 * 1024 * 1024) {
				sizek = 0;
			}
			else {
				uint64_t topmem2 = amd_topmem2();
				basek = 4 * 1024 * 1024;
				sizek = topmem2 / 1024 - basek;
			}
		}

		ram_resource_kb(dev, (idx | i), basek, sizek);
		idx += 0x10;
		printk(BIOS_DEBUG, "node %d: mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
				i, mmio_basek, basek, limitk);
	}

	add_uma_resource_below_tolm(dev, 7);
}

static const char *domain_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name        = domain_acpi_name,
};

static void pre_mp_init(void)
{
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, &mp_ops);

	/* The flash is now no longer cacheable. Reset to WP for performance. */
	mtrr_use_temp_range(OPTIMAL_CACHE_ROM_BASE, OPTIMAL_CACHE_ROM_SIZE,
			    MTRR_TYPE_WRPROT);
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = noop_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = mp_cpu_bus_init,
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
