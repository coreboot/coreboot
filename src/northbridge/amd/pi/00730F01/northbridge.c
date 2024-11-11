/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <acpi/acpi_ivrs.h>
#include <arch/ioapic.h>
#include <arch/vga.h>
#include <types.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <static.h>
#include <string.h>
#include <stdlib.h>
#include <cpu/x86/mp.h>
#include <Porting.h>
#include <Topology.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <acpi/acpigen.h>
#include <northbridge/amd/nb_common.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <southbridge/amd/pi/hudson/pci_devs.h>
#include <amdblocks/cpu.h>

#define PCIE_CAP_AER		BIT(5)
#define PCIE_CAP_ACS		BIT(6)

static int get_dram_base_limit(resource_t *basek, resource_t *limitk)
{
	u32 temp;

	temp = pci_read_config32(DEV_PTR(ht_1), 0x40); //[39:24] at [31:16]
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
	temp = pci_read_config32(DEV_PTR(ht_1), 0x44); //[39:24] at [31:16]
	*limitk = ((temp & 0xffff0000) | 0xffff) >> (10 - 8);
	*limitk += 1; // round up last byte

	return 1;
}

static void add_fixed_resources(struct device *dev, int index)
{
	/* Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xfffff: option ROMs and SeaBIOS (if used)
	 */
	mmio_resource_kb(dev, index++, VGA_MMIO_BASE >> 10, VGA_MMIO_SIZE >> 10);
	reserved_ram_resource_kb(dev, index++, 0xc0000 >> 10, (0x100000 - 0xc0000) >> 10);

	/* Check if CC6 save area is enabled (bit 18 CC6SaveEn)  */
	if (pci_read_config32(DEV_PTR(ht_2), 0x118) & (1 << 18)) {
		/* Add CC6 DRAM UC resource residing at DRAM Limit of size 16MB as per BKDG */
		resource_t basek, limitk;
		if (!get_dram_base_limit(&basek, &limitk))
			return;
		mmio_resource_kb(dev, index++, limitk, 16 * 1024);
	}
}

static void nb_read_resources(struct device *dev)
{
	/*
	 * This MMCONF resource must be reserved in the PCI domain.
	 * It is not honored by the coreboot resource allocator if it is in
	 * the CPU_CLUSTER.
	 */
	mmconf_resource(dev, MMIO_CONF_BASE);

	/* NB IOAPIC2 resource */
	mmio_range(dev, IO_APIC2_ADDR, IO_APIC2_ADDR, 0x1000);

	add_fixed_resources(dev, 0);
}

static void northbridge_init(struct device *dev)
{
	register_new_ioapic(IO_APIC2_ADDR);
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

static unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current)
{
	/* 8-byte IVHD structures must be aligned to the 8-byte boundary. */
	current = ALIGN_UP(current, 8);
	ivrs_ivhd_special_t *ivhd_ioapic = (ivrs_ivhd_special_t *)current;

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->reserved = 0x0000;
	ivhd_ioapic->dte_setting = IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
				   IVHD_DTE_SYS_MGT_NO_TRANS | IVHD_DTE_NMI_PASS |
				   IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS;
	ivhd_ioapic->handle = get_ioapic_id(IO_APIC_ADDR);
	ivhd_ioapic->source_dev_id = PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC);
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	ivhd_ioapic = (ivrs_ivhd_special_t *)current;
	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->reserved = 0x0000;
	ivhd_ioapic->dte_setting = 0x00;
	ivhd_ioapic->handle = get_ioapic_id(IO_APIC2_ADDR);
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
		ivhd_entry->dev_id = dev->path.pci.devfn | (dev->upstream->secondary << 8);
		ivhd_entry->dte_setting = data;
		*current += sizeof(ivrs_ivhd_generic_t);
	} else if (type == IVHD_DEV_8_BYTE_ALIAS_SELECT) {
		/* 8-byte IVHD structures must be aligned to the 8-byte boundary. */
		*current = ALIGN_UP(*current, 8);
		ivrs_ivhd_alias_t *ivhd_entry = (ivrs_ivhd_alias_t *)*current;

		ivhd_entry->type = type;
		ivhd_entry->dev_id = dev->path.pci.devfn | (dev->upstream->secondary << 8);
		ivhd_entry->dte_setting = data;
		ivhd_entry->reserved1 = 0;
		ivhd_entry->reserved2 = 0;
		ivhd_entry->source_dev_id = parent->path.pci.devfn |
					    (parent->upstream->secondary << 8);
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

	if (!root_level) {
		root_level = malloc(sizeof(int8_t));
		*root_level = -1;
	}

	if (dev->path.type == DEVICE_PATH_PCI) {
		if ((dev->upstream->secondary == 0x0) &&
		    (dev->path.pci.devfn == 0x0))
			*root_level = depth;

		if ((*root_level != -1) && (dev->enabled)) {
			if (depth != *root_level)
				ivrs_add_device_or_bridge(parent, dev, current, ivhd_length);
		}
	}

	if (dev->downstream) {
		for (sibling = dev->downstream->children; sibling; sibling = sibling->sibling)
			add_ivhd_device_entries(dev, sibling, depth + 1, depth, root_level,
						current, ivhd_length);
	}

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
	ivhd_11->device_id = 0x02 | (nb_dev->upstream->secondary << 8);
	/* PCI Capability block 0x40 (type 0xf, "Secure device") */
	ivhd_11->capability_offset = 0x40;
	ivhd_11->iommu_base_low = ivrs_agesa->ivhd.iommu_base_low;
	ivhd_11->iommu_base_high = ivrs_agesa->ivhd.iommu_base_high;
	ivhd_11->pci_segment_group = nb_dev->upstream->segment_group;
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
		ivrs->ivhd.device_id = 0x02 | (nb_dev->upstream->secondary << 8);
		/* PCI Capability block 0x40 (type 0xf, "Secure device") */
		ivrs->ivhd.capability_offset = 0x40;
		ivrs->ivhd.iommu_base_low = ivrs_agesa->ivhd.iommu_base_low;
		ivrs->ivhd.iommu_base_high = ivrs_agesa->ivhd.iommu_base_high;
		ivrs->ivhd.pci_segment_group = nb_dev->upstream->segment_group;
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
	char pscope[] = "\\_SB.PCI0";

	acpigen_write_scope(pscope);
	acpigen_write_name_dword("TOM1", get_top_of_mem_below_4gb());

	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", get_top_of_mem_above_4gb() >> 20);
	acpigen_pop_len();
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
	current = ALIGN_UP(current, 8);
	acpi_write_hest((void *)current, acpi_fill_hest);
	acpi_add_table(rsdp, (void *)current);
	current += ((acpi_header_t *)current)->length;

	/* IVRS */
	current = ALIGN_UP(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * IVRS at %lx\n", current);
	ivrs = (acpi_ivrs_t *)current;
	acpi_create_ivrs(ivrs, acpi_fill_ivrs);
	current += ivrs->header.length;
	acpi_add_table(rsdp, ivrs);

	/* SRAT */
	current = ALIGN_UP(current, 8);
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
	current = ALIGN_UP(current, 8);
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
	current = ALIGN_UP(current, 16);
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
	current   = ALIGN_UP(current, 16);
	printk(BIOS_DEBUG, "ACPI:    * SSDT at %lx\n", current);
	ssdt = (acpi_header_t *)agesawrapper_getlateinitptr(PICK_PSTATE);
	if (ssdt != NULL) {
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

struct device_operations amd_pi_northbridge_ops = {
	.read_resources	  = nb_read_resources,
	.set_resources	  = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = northbridge_init,
	.ops_pci           = &pci_dev_ops_pci,
	.acpi_fill_ssdt   = northbridge_fill_ssdt_generator,
	.write_acpi_tables = agesa_write_acpi_tables,
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

struct hw_mem_hole_info {
	unsigned int hole_startk;
	int node_id;
};
static struct hw_mem_hole_info get_hw_mem_hole_info(void)
{
	struct hw_mem_hole_info mem_hole;
	mem_hole.node_id = -1;

	resource_t basek, limitk;
	if (get_dram_base_limit(&basek, &limitk)) { // memory on this node
		u32 hole = pci_read_config32(DEV_PTR(ht_1), 0xf0);
		if (hole & 2) { // we find the hole
			mem_hole.hole_startk = (hole & (0xff << 24)) >> 10;
			mem_hole.node_id = 0; // record the node No with hole
		}
	}
	return mem_hole;
}

static void domain_read_resources(struct device *dev)
{
	unsigned long mmio_basek;
	unsigned long idx = 0;
	struct hw_mem_hole_info mem_hole;
	resource_t basek = 0;
	resource_t limitk = 0;
	resource_t sizek;

	pci_domain_read_resources(dev);

	/* TOP_MEM MSR is our boundary between DRAM and MMIO under 4G */
	mmio_basek = get_top_of_mem_below_4gb() >> 10;

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

	get_dram_base_limit(&basek, &limitk);
	sizek = limitk - basek;

	printk(BIOS_DEBUG, "basek=%08llx, limitk=%08llx, sizek=%08llx,\n",
			   basek, limitk, sizek);

	/* See if we need a hole from 0xa0000 (640K) to 0xfffff (1024K) */
	if (basek < 640 && sizek > 1024) {
		ram_resource_kb(dev, idx++, basek, 640 - basek);
		basek = 1024;
		sizek = limitk - basek;
	}

	printk(BIOS_DEBUG, "basek=%08llx, limitk=%08llx, sizek=%08llx,\n",
			   basek, limitk, sizek);

	/* split the region to accommodate pci memory space */
	if ((basek < 4 * 1024 * 1024) && (limitk > mmio_basek)) {
		if (basek <= mmio_basek) {
			unsigned int pre_sizek;
			pre_sizek = mmio_basek - basek;
			if (pre_sizek > 0) {
				ram_resource_kb(dev, idx++, basek, pre_sizek);
				sizek -= pre_sizek;
			}
			basek = mmio_basek;
		}
		if ((basek + sizek) <= 4 * 1024 * 1024) {
			sizek = 0;
		} else {
			uint64_t topmem2 = get_top_of_mem_above_4gb();
			basek = 4 * 1024 * 1024;
			sizek = topmem2 / 1024 - basek;
		}
	}

	ram_resource_kb(dev, idx++, basek, sizek);
	printk(BIOS_DEBUG, "mmio_basek=%08lx, basek=%08llx, limitk=%08llx\n",
			mmio_basek, basek, limitk);

	add_uma_resource_below_tolm(dev, idx++);
}

static const char *domain_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	return NULL;
}

struct device_operations amd_fam16_mod30_pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.scan_bus	  = pci_host_bridge_scan_bus,
	.acpi_name        = domain_acpi_name,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	extern const struct mp_ops amd_mp_ops_no_smm;
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, &amd_mp_ops_no_smm);

	/* The flash is now no longer cacheable. Reset to WP for performance. */
	mtrr_use_temp_range(OPTIMAL_CACHE_ROM_BASE, OPTIMAL_CACHE_ROM_SIZE,
			    MTRR_TYPE_WRPROT);
}

void generate_cpu_entries(const struct device *device)
{
	int cpu;
	const int cores = get_cpu_count();

	printk(BIOS_DEBUG, "ACPI \\_SB report %d core(s)\n", cores);

	/* Generate \_SB.Pxxx */
	for (cpu = 0; cpu < cores; cpu++) {
		acpigen_write_processor_device(cpu);
		acpigen_write_processor_device_end();
	}
}

struct device_operations amd_fam16_mod30_cpu_bus_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.init		= mp_cpu_bus_init,
	.acpi_fill_ssdt	= generate_cpu_entries,
};

struct chip_operations northbridge_amd_pi_00730F01_ops = {
	.name = "AMD FAM16 Root Complex",
	.final = fam16_finalize,
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
