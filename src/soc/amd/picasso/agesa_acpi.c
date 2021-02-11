/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_crat.h>
#include <acpi/acpi_ivrs.h>
#include <console/console.h>
#include <cpu/amd/cpuid.h>
#include <cpu/amd/msr.h>
#include <fsp/util.h>
#include <FspGuids.h>
#include <soc/acpi.h>
#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <amdblocks/cpu.h>
#include <amdblocks/ioapic.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <stdlib.h>
#include <arch/mmio.h>

struct amd_fsp_acpi_hob_info {
	uint32_t table_size_in_bytes;
	uint8_t total_hobs_for_table;
	uint8_t sequence_number;
	uint16_t reserved;
	uint16_t hob_payload[0xffc8];
} __packed;

static uintptr_t add_agesa_acpi_table(guid_t guid, const char *name, acpi_rsdp_t *rsdp,
				      uintptr_t current)
{
	const struct amd_fsp_acpi_hob_info *data;
	void *table = (void *)current;
	size_t hob_size;

	data = fsp_find_extension_hob_by_guid(guid.b, &hob_size);
	if (!data) {
		printk(BIOS_ERR, "AGESA %s ACPI table was not found.\n", name);
		return current;
	}

	printk(BIOS_INFO, "ACPI:    * %s (AGESA).\n", name);

	memcpy(table, data->hob_payload, data->table_size_in_bytes);

	current += data->table_size_in_bytes;
	acpi_add_table(rsdp, table);
	current = acpi_align_current(current);

	return current;
}

unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current)
{
	ivrs_ivhd_special_t *ivhd_ioapic = (ivrs_ivhd_special_t *)current;
	memset(ivhd_ioapic, 0, sizeof(*ivhd_ioapic));

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->dte_setting = IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
				   IVHD_DTE_SYS_MGT_NO_TRANS | IVHD_DTE_NMI_PASS |
				   IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS;
	ivhd_ioapic->handle = FCH_IOAPIC_ID;
	ivhd_ioapic->source_dev_id = PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC);
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	ivhd_ioapic = (ivrs_ivhd_special_t *)current;
	memset(ivhd_ioapic, 0, sizeof(*ivhd_ioapic));

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->handle = GNB_IOAPIC_ID;
	ivhd_ioapic->source_dev_id = PCI_DEVFN(0, 1);
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	return current;
}

static unsigned long ivhd_describe_hpet(unsigned long current)
{
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

static unsigned long ivhd_describe_f0_device(unsigned long current,
						uint16_t dev_id, uint8_t datasetting)
{
	ivrs_ivhd_f0_entry_t *ivhd_f0 = (ivrs_ivhd_f0_entry_t *) current;

	ivhd_f0->type = IVHD_DEV_VARIABLE;
	ivhd_f0->dev_id = dev_id;
	ivhd_f0->dte_setting = datasetting;
	ivhd_f0->hardware_id[0] = 'A';
	ivhd_f0->hardware_id[1] = 'M';
	ivhd_f0->hardware_id[2] = 'D';
	ivhd_f0->hardware_id[3] = 'I';
	ivhd_f0->hardware_id[4] = '0';
	ivhd_f0->hardware_id[5] = '0';
	ivhd_f0->hardware_id[6] = '4';
	ivhd_f0->hardware_id[7] = '0';

	memset(ivhd_f0->compatible_id, 0, sizeof(ivhd_f0->compatible_id));

	ivhd_f0->uuid_format = 0;
	ivhd_f0->uuid_length = 0;

	current += sizeof(ivrs_ivhd_f0_entry_t);
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

	if (!root_level)
		return;

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
}

static unsigned long acpi_fill_ivrs40(unsigned long current, acpi_ivrs_t *ivrs)
{
	acpi_ivrs_ivhd40_t *ivhd_40;
	unsigned long current_backup;
	int8_t root_level;

	/*
	 * These devices should be already found by previous function.
	 * Do not perform NULL checks.
	 */
	struct device *nb_dev = pcidev_on_root(0, 0);
	struct device *iommu_dev = pcidev_on_root(0, 2);

	memset((void *)current, 0, sizeof(acpi_ivrs_ivhd40_t));
	ivhd_40 = (acpi_ivrs_ivhd40_t *)current;

	/* Enable EFR */
	ivhd_40->type = IVHD_BLOCK_TYPE_FULL__ACPI_HID;
	/* For type 40h bits 6 and 7 are reserved */
	ivhd_40->flags = ivrs->ivhd.flags & 0x3f;
	ivhd_40->length = sizeof(struct acpi_ivrs_ivhd_40);
	/* BDF <bus>:00.2 */
	ivhd_40->device_id = 0x02 | (nb_dev->bus->secondary << 8);
	ivhd_40->capability_offset = pci_find_capability(iommu_dev, IOMMU_CAP_ID);
	ivhd_40->iommu_base_low = ivrs->ivhd.iommu_base_low;
	ivhd_40->iommu_base_high = ivrs->ivhd.iommu_base_high;
	ivhd_40->pci_segment_group = 0x0000;
	ivhd_40->iommu_info = ivrs->ivhd.iommu_info;
	/* For type 40h bits 31:28 and 12:0 are reserved */
	ivhd_40->iommu_attributes = ivrs->ivhd.iommu_feature_info & 0xfffe000;

	if (pci_read_config32(iommu_dev, ivhd_40->capability_offset) & EFR_FEATURE_SUP) {
		ivhd_40->efr_reg_image_low  = read32((void *)ivhd_40->iommu_base_low + 0x30);
		ivhd_40->efr_reg_image_high = read32((void *)ivhd_40->iommu_base_low + 0x34);
	}

	current += sizeof(acpi_ivrs_ivhd40_t);

	/* Now repeat all the device entries from type 10h */
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), PCI_DEVFN(0x1f, 6), 0);
	ivhd_40->length += (current - current_backup);
	root_level = -1;
	add_ivhd_device_entries(NULL, all_devices, 0, -1, &root_level,
		&current, &ivhd_40->length);

	/* Describe HPET */
	current_backup = current;
	current = ivhd_describe_hpet(current);
	ivhd_40->length += (current - current_backup);

	/* Describe IOAPICs */
	current_backup = current;
	current = acpi_fill_ivrs_ioapic(ivrs, current);
	ivhd_40->length += (current - current_backup);

	/* Describe EMMC */
	current_backup = current;
	current = ivhd_describe_f0_device(current, PCI_DEVFN(0x13, 1),
				IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
				IVHD_DTE_SYS_MGT_TRANS   | IVHD_DTE_NMI_PASS |
				IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS);
	ivhd_40->length += (current - current_backup);

	return current;
}

static unsigned long acpi_fill_ivrs11(unsigned long current, acpi_ivrs_t *ivrs)
{
	acpi_ivrs_ivhd11_t *ivhd_11;
	ivhd11_iommu_attr_t *ivhd11_attr_ptr;
	unsigned long current_backup;
	int8_t root_level;

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
	ivhd_11->flags = ivrs->ivhd.flags & 0x3f;
	ivhd_11->length = sizeof(struct acpi_ivrs_ivhd_11);
	/* BDF <bus>:00.2 */
	ivhd_11->device_id = 0x02 | (nb_dev->bus->secondary << 8);
	ivhd_11->capability_offset = pci_find_capability(iommu_dev, IOMMU_CAP_ID);
	ivhd_11->iommu_base_low = ivrs->ivhd.iommu_base_low;
	ivhd_11->iommu_base_high = ivrs->ivhd.iommu_base_high;
	ivhd_11->pci_segment_group = 0x0000;
	ivhd_11->iommu_info = ivrs->ivhd.iommu_info;
	ivhd11_attr_ptr = (ivhd11_iommu_attr_t *) &ivrs->ivhd.iommu_feature_info;
	ivhd_11->iommu_attributes.perf_counters = ivhd11_attr_ptr->perf_counters;
	ivhd_11->iommu_attributes.perf_counter_banks = ivhd11_attr_ptr->perf_counter_banks;
	ivhd_11->iommu_attributes.msi_num_ppr = ivhd11_attr_ptr->msi_num_ppr;

	if (pci_read_config32(iommu_dev, ivhd_11->capability_offset) & EFR_FEATURE_SUP) {
		ivhd_11->efr_reg_image_low  = read32((void *)ivhd_11->iommu_base_low + 0x30);
		ivhd_11->efr_reg_image_high = read32((void *)ivhd_11->iommu_base_low + 0x34);
	}

	current += sizeof(acpi_ivrs_ivhd11_t);

	/* Now repeat all the device entries from type 10h */
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), PCI_DEVFN(0x1f, 6), 0);
	ivhd_11->length += (current - current_backup);
	root_level = -1;
	add_ivhd_device_entries(NULL, all_devices, 0, -1, &root_level,
		&current, &ivhd_11->length);

	/* Describe HPET */
	current_backup = current;
	current = ivhd_describe_hpet(current);
	ivhd_11->length += (current - current_backup);

	/* Describe IOAPICs */
	current_backup = current;
	current = acpi_fill_ivrs_ioapic(ivrs, current);
	ivhd_11->length += (current - current_backup);

	return acpi_fill_ivrs40(current, ivrs);
}

static unsigned long acpi_fill_ivrs(acpi_ivrs_t *ivrs, unsigned long current)
{
	unsigned long current_backup;
	uint64_t mmio_x30_value;
	uint64_t mmio_x18_value;
	uint64_t mmio_x4000_value;
	uint32_t cap_offset_0;
	uint32_t cap_offset_10;
	int8_t root_level;

	struct device *iommu_dev;
	struct device *nb_dev;

	nb_dev = pcidev_on_root(0, 0);
	if (!nb_dev) {
		printk(BIOS_WARNING, "%s: Northbridge device not present!\n", __func__);
		printk(BIOS_WARNING, "%s: IVRS table not generated...\n", __func__);

		return (unsigned long)ivrs;
	}

	iommu_dev = pcidev_on_root(0, 2);
	if (!iommu_dev) {
		printk(BIOS_WARNING, "%s: IOMMU device not found\n", __func__);

		return (unsigned long)ivrs;
	}

	if (ivrs != NULL) {
		ivrs->ivhd.type = IVHD_BLOCK_TYPE_LEGACY__FIXED;
		ivrs->ivhd.length = sizeof(struct acpi_ivrs_ivhd);

		/* BDF <bus>:00.2 */
		ivrs->ivhd.device_id = 0x02 | (nb_dev->bus->secondary << 8);
		ivrs->ivhd.capability_offset = pci_find_capability(iommu_dev, IOMMU_CAP_ID);
		ivrs->ivhd.iommu_base_low = pci_read_config32(iommu_dev, 0x44) & 0xffffc000;
		ivrs->ivhd.iommu_base_high = pci_read_config32(iommu_dev, 0x48);

		cap_offset_0 = pci_read_config32(iommu_dev, ivrs->ivhd.capability_offset);
		cap_offset_10 = pci_read_config32(iommu_dev,
						ivrs->ivhd.capability_offset + 0x10);
		mmio_x18_value = read64((void *)ivrs->ivhd.iommu_base_low + 0x18);
		mmio_x30_value = read64((void *)ivrs->ivhd.iommu_base_low + 0x30);
		mmio_x4000_value = read64((void *)ivrs->ivhd.iommu_base_low + 0x4000);

		ivrs->ivhd.flags |= ((mmio_x30_value & MMIO_EXT_FEATURE_PPR_SUP) ?
							IVHD_FLAG_PPE_SUP : 0);
		ivrs->ivhd.flags |= ((mmio_x30_value & MMIO_EXT_FEATURE_PRE_F_SUP) ?
							IVHD_FLAG_PREF_SUP : 0);
		ivrs->ivhd.flags |= ((mmio_x18_value & MMIO_CTRL_COHERENT) ?
							IVHD_FLAG_COHERENT : 0);
		ivrs->ivhd.flags |= ((cap_offset_0 & CAP_OFFSET_0_IOTLB_SP) ?
							IVHD_FLAG_IOTLB_SUP : 0);
		ivrs->ivhd.flags |= ((mmio_x18_value & MMIO_CTRL_ISOC) ?
							IVHD_FLAG_ISOC : 0);
		ivrs->ivhd.flags |= ((mmio_x18_value & MMIO_CTRL_RES_PASS_PW) ?
							IVHD_FLAG_RES_PASS_PW : 0);
		ivrs->ivhd.flags |= ((mmio_x18_value & MMIO_CTRL_PASS_PW) ?
							IVHD_FLAG_PASS_PW : 0);
		ivrs->ivhd.flags |= ((mmio_x18_value & MMIO_CTRL_HT_TUN_EN) ?
							IVHD_FLAG_HT_TUN_EN : 0);

		ivrs->ivhd.pci_segment_group = 0x0000;

		ivrs->ivhd.iommu_info = pci_read_config16(iommu_dev,
			ivrs->ivhd.capability_offset + 0x10) & 0x1F;
		ivrs->ivhd.iommu_info |= (pci_read_config16(iommu_dev,
			ivrs->ivhd.capability_offset + 0xC) & 0x1F) << IOMMU_INFO_UNIT_ID_SHIFT;

		ivrs->ivhd.iommu_feature_info = 0;
		ivrs->ivhd.iommu_feature_info |= (mmio_x30_value & MMIO_EXT_FEATURE_HATS_MASK)
			<< (IOMMU_FEATURE_HATS_SHIFT - MMIO_EXT_FEATURE_HATS_SHIFT);

		ivrs->ivhd.iommu_feature_info |= (mmio_x30_value & MMIO_EXT_FEATURE_GATS_MASK)
			<< (IOMMU_FEATURE_GATS_SHIFT - MMIO_EXT_FEATURE_GATS_SHIFT);

		ivrs->ivhd.iommu_feature_info |= (cap_offset_10 & CAP_OFFSET_10_MSI_NUM_PPR)
			>> (CAP_OFFSET_10_MSI_NUM_PPR_SHIFT
				- IOMMU_FEATURE_MSI_NUM_PPR_SHIFT);

		ivrs->ivhd.iommu_feature_info |= (mmio_x4000_value &
			MMIO_CNT_CFG_N_COUNTER_BANKS)
			<< (IOMMU_FEATURE_PN_BANKS_SHIFT - MMIO_CNT_CFG_N_CNT_BANKS_SHIFT);

		ivrs->ivhd.iommu_feature_info |= (mmio_x4000_value & MMIO_CNT_CFG_N_COUNTER)
			<< (IOMMU_FEATURE_PN_COUNTERS_SHIFT - MMIO_CNT_CFG_N_COUNTER_SHIFT);
		ivrs->ivhd.iommu_feature_info |= (mmio_x30_value &
			MMIO_EXT_FEATURE_PAS_MAX_MASK)
			>> (MMIO_EXT_FEATURE_PAS_MAX_SHIFT - IOMMU_FEATURE_PA_SMAX_SHIFT);
		ivrs->ivhd.iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_HE_SUP)
			? IOMMU_FEATURE_HE_SUP : 0);
		ivrs->ivhd.iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_GA_SUP)
			? IOMMU_FEATURE_GA_SUP : 0);
		ivrs->ivhd.iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_IA_SUP)
			? IOMMU_FEATURE_IA_SUP : 0);
		ivrs->ivhd.iommu_feature_info |= (mmio_x30_value &
			MMIO_EXT_FEATURE_GLX_SUP_MASK)
			>> (MMIO_EXT_FEATURE_GLX_SHIFT - IOMMU_FEATURE_GLX_SHIFT);
		ivrs->ivhd.iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_GT_SUP)
			? IOMMU_FEATURE_GT_SUP : 0);
		ivrs->ivhd.iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_NX_SUP)
			? IOMMU_FEATURE_NX_SUP : 0);
		ivrs->ivhd.iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_XT_SUP)
			? IOMMU_FEATURE_XT_SUP : 0);

		/* Enable EFR if supported */
		ivrs->iv_info = pci_read_config32(iommu_dev,
					ivrs->ivhd.capability_offset + 0x10) & 0x007fffe0;
		if (pci_read_config32(iommu_dev,
					ivrs->ivhd.capability_offset) & EFR_FEATURE_SUP)
			ivrs->iv_info |= IVINFO_EFR_SUPPORTED;

	} else {
		printk(BIOS_WARNING, "%s: AGESA returned NULL IVRS\n", __func__);

		return (unsigned long)ivrs;
	}

	/*
	 * Add all possible PCI devices on bus 0 that can generate transactions
	 * processed by IOMMU. Start with device 00:01.0
	*/
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), PCI_DEVFN(0x1f, 6), 0);
	ivrs->ivhd.length += (current - current_backup);
	root_level = -1;
	add_ivhd_device_entries(NULL, all_devices, 0, -1, &root_level,
		&current, &ivrs->ivhd.length);

	/* Describe HPET */
	current_backup = current;
	current = ivhd_describe_hpet(current);
	ivrs->ivhd.length += (current - current_backup);

	/* Describe IOAPICs */
	current_backup = current;
	current = acpi_fill_ivrs_ioapic(ivrs, current);
	ivrs->ivhd.length += (current - current_backup);

	/* If EFR is not supported, IVHD type 11h is reserved */
	if (!(ivrs->iv_info & IVINFO_EFR_SUPPORTED))
		return current;

	return acpi_fill_ivrs11(current, ivrs);
}

static unsigned long gen_crat_hsa_entry(struct acpi_crat_header *crat, unsigned long current)
{
	struct crat_hsa_processing_unit *hsa_entry = (struct crat_hsa_processing_unit *)current;
	memset(hsa_entry, 0, sizeof(struct crat_hsa_processing_unit));

	hsa_entry->flags = CRAT_HSA_PR_FLAG_EN | CRAT_HSA_PR_FLAG_CPU_PRES;
	hsa_entry->wave_front_size = 4;
	hsa_entry->num_cpu_cores = get_cpu_count();
	hsa_entry->length = sizeof(struct crat_hsa_processing_unit);
	crat->total_entries++;

	current += hsa_entry->length;
	return current;
}

static unsigned long create_crat_memory_entry(uint32_t domain, uint64_t region_base,
					      uint64_t region_size, unsigned long current)
{
	struct crat_memory *mem_affinity = (struct crat_memory *)current;
	memset(mem_affinity, 0, sizeof(struct crat_memory));

	mem_affinity->type = CRAT_MEMORY_TYPE;
	mem_affinity->length = sizeof(struct crat_memory);
	mem_affinity->proximity_domain = 0;
	mem_affinity->base_address_low = region_base & 0xffffffff;
	mem_affinity->base_address_high = (region_base >> 32) & 0xffffffff;
	mem_affinity->length_low = region_size & 0xffffffff;
	mem_affinity->length_high = (region_size >> 32) & 0xffffffff;
	mem_affinity->flags = CRAT_MEM_FLAG_EN;
	mem_affinity->width = 64;

	current += mem_affinity->length;
	return current;
}

static unsigned long gen_crat_memory_entries(struct acpi_crat_header *crat,
					     unsigned long current)
{
	uint32_t dram_base_reg, dram_limit_reg, dram_hole_ctl;
	uint64_t memory_length, memory_base, hole_base, size_below_hole;
	size_t new_entries = 0;

	for (size_t dram_map_idx = 0; dram_map_idx < PICASSO_NUM_DRAM_REG;
	     dram_map_idx++) {
		dram_base_reg =
			data_fabric_read32(0, DF_DRAM_BASE(dram_map_idx), IOMS0_FABRIC_ID);

		if (dram_base_reg & DRAM_BASE_REG_VALID) {
			dram_limit_reg = data_fabric_read32(0, DF_DRAM_LIMIT(dram_map_idx),
								IOMS0_FABRIC_ID);
			memory_length =
				((dram_limit_reg & DRAM_LIMIT_ADDR) >> DRAM_LIMIT_ADDR_SHFT) + 1
				- ((dram_base_reg & DRAM_BASE_ADDR) >> DRAM_BASE_ADDR_SHFT);
			memory_length = memory_length << 28;
			memory_base = (dram_base_reg & DRAM_BASE_ADDR)
				      << (28 - DRAM_BASE_ADDR_SHFT);

			if (memory_base == 0) {
				current =
					create_crat_memory_entry(0, 0ull, 0xa0000ull, current);
				memory_base = (1 * 1024 * 1024);
				memory_length = memory_base;
				new_entries++;
			}

			if (dram_base_reg & DRAM_BASE_HOLE_EN) {
				dram_hole_ctl = data_fabric_read32(0, D18F0_DRAM_HOLE_CTL,
								       IOMS0_FABRIC_ID);
				hole_base = (dram_hole_ctl & DRAM_HOLE_CTL_BASE);
				size_below_hole = hole_base - memory_base;
				current = create_crat_memory_entry(0, memory_base,
								   size_below_hole, current);
				memory_length = (((dram_limit_reg & DRAM_LIMIT_ADDR)
						  >> DRAM_LIMIT_ADDR_SHFT)
						 + 1 - 0x10)
						<< 28;
				memory_base = 0x100000000;
				new_entries++;
			}

			current = create_crat_memory_entry(0, memory_base, memory_length,
							   current);
			new_entries++;
		}
	}
	crat->total_entries += new_entries;
	return current;
}

static unsigned long add_crat_cache_entry(struct crat_cache **cache_affinity,
					  unsigned long current)
{
	*cache_affinity = (struct crat_cache *)current;
	memset(*cache_affinity, 0, sizeof(struct crat_cache));

	(*cache_affinity)->type = CRAT_CACHE_TYPE;
	(*cache_affinity)->length = sizeof(struct crat_cache);
	(*cache_affinity)->flags = CRAT_CACHE_FLAG_EN | CRAT_CACHE_FLAG_CPU_CACHE;

	current += sizeof(struct crat_cache);
	return current;
}

static uint8_t get_associativity(uint32_t encoded_associativity)
{
	uint8_t associativity = 0;

	switch (encoded_associativity) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return encoded_associativity;
	case 5:
		associativity = 6;
		break;
	case 6:
		associativity = 8;
		break;
	case 8:
		associativity = 16;
		break;
	case 0xA:
		associativity = 32;
		break;
	case 0xB:
		associativity = 48;
		break;
	case 0xC:
		associativity = 64;
		break;
	case 0xD:
		associativity = 96;
		break;
	case 0xE:
		associativity = 128;
		break;
	case 0xF:
		associativity = 0xFF;
		break;
	default:
		return 0;
	}

	return associativity;
}

static unsigned long gen_crat_cache_entry(struct acpi_crat_header *crat, unsigned long current)
{
	size_t total_num_threads, num_threads_sharing0, num_threads_sharing1,
		num_threads_sharing2, num_threads_sharing3, thread, new_entries;
	struct cpuid_result cache_props0, cache_props1, cache_props2, cache_props3;
	uint8_t sibling_mask = 0;
	uint32_t l1_data_cache_ids, l1_inst_cache_ids, l2_cache_ids, l3_cache_ids;
	struct crat_cache *cache_affinity = NULL;

	total_num_threads = get_cpu_count();

	cache_props0 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_0);
	cache_props1 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_1);
	cache_props2 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_2);
	cache_props3 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_3);

	l1_data_cache_ids = cpuid_ecx(CPUID_L1_TLB_CACHE_IDS);
	l1_inst_cache_ids = cpuid_edx(CPUID_L1_TLB_CACHE_IDS);
	l2_cache_ids = cpuid_ecx(CPUID_L2_L3_CACHE_L2_TLB_IDS);
	l3_cache_ids = cpuid_edx(CPUID_L2_L3_CACHE_L2_TLB_IDS);

	num_threads_sharing0 =
		((cache_props0.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing1 =
		((cache_props1.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing2 =
		((cache_props2.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing3 =
		((cache_props3.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;

	new_entries = 0;
	for (thread = 0; thread < total_num_threads; thread++) {
		/* L1 data cache */
		if (thread % num_threads_sharing0 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |= CRAT_CACHE_FLAG_DATA_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing0; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props0.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				(l1_data_cache_ids & L1_DC_SIZE_MASK) >> L1_DC_SIZE_SHFT;
			cache_affinity->cache_level = CRAT_L1_CACHE;
			cache_affinity->lines_per_tag =
				(l1_data_cache_ids & L1_DC_LINE_TAG_MASK)
				>> L1_DC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l1_data_cache_ids & L1_DC_LINE_SIZE_MASK)
				>> L1_DC_LINE_SIZE_SHFT;
			cache_affinity->associativity =
				(l1_data_cache_ids & L1_DC_ASSOC_MASK) >> L1_DC_ASSOC_SHFT;
			cache_affinity->cache_latency = 1;
		}

		/* L1 instruction cache */
		if (thread % num_threads_sharing1 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |= CRAT_CACHE_FLAG_INSTR_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing1; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props1.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				(l1_inst_cache_ids & L1_IC_SIZE_MASK) >> L1_IC_SIZE_SHFT;
			cache_affinity->cache_level = CRAT_L1_CACHE;
			cache_affinity->lines_per_tag =
				(l1_inst_cache_ids & L1_IC_LINE_TAG_MASK)
				>> L1_IC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l1_inst_cache_ids & L1_IC_LINE_SIZE_MASK)
				>> L1_IC_LINE_SIZE_SHFT;
			cache_affinity->associativity =
				(l1_inst_cache_ids & L1_IC_ASSOC_MASK) >> L1_IC_ASSOC_SHFT;
			cache_affinity->cache_latency = 1;
		}

		/* L2 cache */
		if (thread % num_threads_sharing2 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |=
				CRAT_CACHE_FLAG_DATA_CACHE | CRAT_CACHE_FLAG_INSTR_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing2; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props2.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				(l2_cache_ids & L2_DC_SIZE_MASK) >> L2_DC_SIZE_SHFT;
			cache_affinity->cache_level = CRAT_L2_CACHE;
			cache_affinity->lines_per_tag =
				(l2_cache_ids & L2_DC_LINE_TAG_MASK) >> L2_DC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l2_cache_ids & L2_DC_LINE_SIZE_MASK) >> L2_DC_LINE_SIZE_SHFT;
			cache_affinity->associativity = get_associativity(
				(l2_cache_ids & L2_DC_ASSOC_MASK) >> L2_DC_ASSOC_SHFT);
			cache_affinity->cache_latency = 1;
		}

		/* L3 cache */
		if (thread % num_threads_sharing3 == 0) {
			current = add_crat_cache_entry(&cache_affinity, current);
			new_entries++;

			cache_affinity->flags |=
				CRAT_CACHE_FLAG_DATA_CACHE | CRAT_CACHE_FLAG_INSTR_CACHE;
			cache_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing3; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			cache_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			cache_affinity->cache_properties =
				(cache_props0.edx & CACHE_INCLUSIVE_MASK) ? 2 : 0;
			cache_affinity->cache_size =
				((l3_cache_ids & L3_DC_SIZE_MASK) >> L3_DC_SIZE_SHFT) * 512;
			cache_affinity->cache_level = CRAT_L3_CACHE;
			cache_affinity->lines_per_tag =
				(l3_cache_ids & L3_DC_LINE_TAG_MASK) >> L3_DC_LINE_TAG_SHFT;
			cache_affinity->cache_line_size =
				(l3_cache_ids & L3_DC_LINE_SIZE_MASK) >> L3_DC_LINE_SIZE_SHFT;
			cache_affinity->associativity = get_associativity(
				(l3_cache_ids & L3_DC_ASSOC_MASK) >> L3_DC_ASSOC_SHFT);
			cache_affinity->cache_latency = 1;
		}
	}
	crat->total_entries += new_entries;
	return current;
}

static uint8_t get_tlb_size(enum tlb_type type, struct crat_tlb *crat_tlb_entry,
			    uint16_t raw_assoc_size)
{
	uint8_t tlbsize;

	if (raw_assoc_size >= 256) {
		tlbsize = (uint8_t)(raw_assoc_size / 256);

		if (type == tlb_2m)
			crat_tlb_entry->flags |= CRAT_TLB_FLAG_2MB_BASE_256;
		else if (type == tlb_4k)
			crat_tlb_entry->flags |= CRAT_TLB_FLAG_4K_BASE_256;
		else if (type == tlb_1g)
			crat_tlb_entry->flags |= CRAT_TLB_FLAG_1GB_BASE_256;
	} else {
		tlbsize = (uint8_t)(raw_assoc_size);
	}
	return tlbsize;
}

static unsigned long add_crat_tlb_entry(struct crat_tlb **tlb_affinity, unsigned long current)
{
	*tlb_affinity = (struct crat_tlb *)current;
	memset(*tlb_affinity, 0, sizeof(struct crat_tlb));

	(*tlb_affinity)->type = CRAT_TLB_TYPE;
	(*tlb_affinity)->length = sizeof(struct crat_tlb);
	(*tlb_affinity)->flags = CRAT_TLB_FLAG_EN | CRAT_TLB_FLAG_CPU_TLB;

	current += sizeof(struct crat_tlb);
	return current;
}

static unsigned long gen_crat_tlb_entry(struct acpi_crat_header *crat, unsigned long current)
{
	size_t total_num_threads, num_threads_sharing0, num_threads_sharing1,
		num_threads_sharing2, thread, new_entries;
	struct cpuid_result cache_props0, cache_props1, cache_props2;
	uint8_t sibling_mask = 0;
	uint32_t l1_tlb_2M4M_ids, l1_tlb_4K_ids, l2_tlb_2M4M_ids, l2_tlb_4K_ids, l1_tlb_1G_ids,
		l2_tlb_1G_ids;
	struct crat_tlb *tlb_affinity = NULL;

	total_num_threads = get_cpu_count();
	cache_props0 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_0);
	cache_props1 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_1);
	cache_props2 = cpuid_ext(CPUID_CACHE_PROPS, CACHE_PROPS_2);

	l1_tlb_2M4M_ids = cpuid_eax(CPUID_L1_TLB_CACHE_IDS);
	l2_tlb_2M4M_ids = cpuid_eax(CPUID_L2_L3_CACHE_L2_TLB_IDS);
	l1_tlb_4K_ids = cpuid_ebx(CPUID_L1_TLB_CACHE_IDS);
	l2_tlb_4K_ids = cpuid_ebx(CPUID_L2_L3_CACHE_L2_TLB_IDS);
	l1_tlb_1G_ids = cpuid_eax(CPUID_TLB_L1L2_1G_IDS);
	l2_tlb_1G_ids = cpuid_ebx(CPUID_TLB_L1L2_1G_IDS);

	num_threads_sharing0 =
		((cache_props0.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing1 =
		((cache_props1.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;
	num_threads_sharing2 =
		((cache_props2.eax & NUM_SHARE_CACHE_MASK) >> NUM_SHARE_CACHE_SHFT) + 1;

	new_entries = 0;
	for (thread = 0; thread < total_num_threads; thread++) {

		/* L1 data TLB */
		if (thread % num_threads_sharing0 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_DATA_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing0; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L1_CACHE;

			tlb_affinity->data_tlb_2mb_assoc =
				(l1_tlb_2M4M_ids & L1_DAT_TLB_2M4M_ASSOC_MASK)
				>> L1_DAT_TLB_2M4M_ASSOC_SHFT;
			tlb_affinity->data_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l1_tlb_2M4M_ids & L1_DAT_TLB_2M4M_SIZE_MASK)
						     >> L1_DAT_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->data_tlb_4k_assoc =
				(l1_tlb_4K_ids & L1_DAT_TLB_4K_ASSOC_MASK)
				>> L1_DAT_TLB_4K_ASSOC_SHFT;
			tlb_affinity->data_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l1_tlb_4K_ids & L1_DAT_TLB_4K_SIZE_MASK)
						     >> L1_DAT_TLB_4K_SIZE_SHFT);

			tlb_affinity->data_tlb_1g_assoc =
				(l1_tlb_1G_ids & L1_DAT_TLB_1G_ASSOC_MASK)
				>> L1_DAT_TLB_1G_ASSOC_SHFT;
			tlb_affinity->data_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l1_tlb_1G_ids & L1_DAT_TLB_1G_SIZE_MASK)
						     >> L1_DAT_TLB_1G_SIZE_SHFT);
		}

		/* L1 instruction TLB */
		if (thread % num_threads_sharing1 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_INSTR_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing1; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L1_CACHE;
			tlb_affinity->instr_tlb_2mb_assoc =
				(l1_tlb_2M4M_ids & L1_INST_TLB_2M4M_ASSOC_MASK)
				>> L1_INST_TLB_2M4M_ASSOC_SHFT;
			tlb_affinity->instr_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l1_tlb_2M4M_ids & L1_INST_TLB_2M4M_SIZE_MASK)
						     >> L1_INST_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->instr_tlb_4k_assoc =
				(l1_tlb_4K_ids & L1_INST_TLB_4K_ASSOC_MASK)
				>> L1_INST_TLB_4K_ASSOC_SHFT;
			tlb_affinity->instr_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l1_tlb_4K_ids & L1_INST_TLB_4K_SIZE_MASK)
						     >> L1_INST_TLB_4K_SIZE_SHFT);

			tlb_affinity->instr_tlb_1g_assoc =
				(l1_tlb_1G_ids & L1_INST_TLB_1G_ASSOC_MASK)
				>> L1_INST_TLB_1G_ASSOC_SHFT;
			tlb_affinity->instr_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l1_tlb_1G_ids & L1_INST_TLB_1G_SIZE_MASK)
						     >> L1_INST_TLB_1G_SIZE_SHFT);
		}

		/* L2 Data TLB */
		if (thread % num_threads_sharing2 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_DATA_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing2; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L2_CACHE;
			tlb_affinity->data_tlb_2mb_assoc =
				(l2_tlb_2M4M_ids & L2_DAT_TLB_2M4M_ASSOC_MASK)
				>> L2_DAT_TLB_2M4M_ASSOC_SHFT;
			tlb_affinity->data_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l2_tlb_2M4M_ids & L2_DAT_TLB_2M4M_SIZE_MASK)
						     >> L2_DAT_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->data_tlb_4k_assoc =
				get_associativity((l2_tlb_4K_ids & L2_DAT_TLB_2M4M_ASSOC_MASK)
						  >> L2_DAT_TLB_4K_ASSOC_SHFT);
			tlb_affinity->data_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l2_tlb_2M4M_ids & L2_DAT_TLB_4K_SIZE_MASK)
						     >> L2_DAT_TLB_4K_SIZE_SHFT);

			tlb_affinity->data_tlb_1g_assoc =
				get_associativity((l2_tlb_1G_ids & L2_DAT_TLB_1G_ASSOC_MASK)
						  >> L2_DAT_TLB_1G_ASSOC_SHFT);
			tlb_affinity->data_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l2_tlb_1G_ids & L2_DAT_TLB_1G_SIZE_MASK)
						     >> L2_DAT_TLB_1G_SIZE_SHFT);
		}

		/* L2 Instruction TLB */
		if (thread % num_threads_sharing2 == 0) {
			current = add_crat_tlb_entry(&tlb_affinity, current);
			new_entries++;

			tlb_affinity->flags |= CRAT_TLB_FLAG_INSTR_TLB;
			tlb_affinity->proc_id_low = thread;
			sibling_mask = 1;
			for (size_t sibling = 1; sibling < num_threads_sharing2; sibling++)
				sibling_mask = (sibling_mask << 1) + 1;
			tlb_affinity->sibling_map[thread / 8] = sibling_mask << (thread % 8);
			tlb_affinity->tlb_level = CRAT_L2_CACHE;
			tlb_affinity->instr_tlb_2mb_assoc = get_associativity(
				(l2_tlb_2M4M_ids & L2_INST_TLB_2M4M_ASSOC_MASK)
				>> L2_INST_TLB_2M4M_ASSOC_SHFT);
			tlb_affinity->instr_tlb_2mb_size =
				get_tlb_size(tlb_2m, tlb_affinity,
					     (l2_tlb_2M4M_ids & L2_INST_TLB_2M4M_SIZE_MASK)
						     >> L2_INST_TLB_2M4M_SIZE_SHFT);

			tlb_affinity->instr_tlb_4k_assoc =
				get_associativity((l2_tlb_4K_ids & L2_INST_TLB_4K_ASSOC_MASK)
						  >> L2_INST_TLB_4K_ASSOC_SHFT);
			tlb_affinity->instr_tlb_4k_size =
				get_tlb_size(tlb_4k, tlb_affinity,
					     (l2_tlb_4K_ids & L2_INST_TLB_4K_SIZE_MASK)
						     >> L2_INST_TLB_4K_SIZE_SHFT);

			tlb_affinity->instr_tlb_1g_assoc =
				get_associativity((l2_tlb_1G_ids & L2_INST_TLB_1G_ASSOC_MASK)
						  >> L2_INST_TLB_1G_ASSOC_SHFT);
			tlb_affinity->instr_tlb_1g_size =
				get_tlb_size(tlb_1g, tlb_affinity,
					     (l2_tlb_1G_ids & L2_INST_TLB_1G_SIZE_MASK)
						     >> L2_INST_TLB_1G_SIZE_SHFT);
		}
	}

	crat->total_entries += new_entries;
	return current;
}

static unsigned long acpi_fill_crat(struct acpi_crat_header *crat, unsigned long current)
{
	current = gen_crat_hsa_entry(crat, current);
	current = gen_crat_memory_entries(crat, current);
	current = gen_crat_cache_entry(crat, current);
	current = gen_crat_tlb_entry(crat, current);
	crat->num_nodes++;

	return current;
}

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp)
{
	acpi_ivrs_t *ivrs;
	struct acpi_crat_header *crat;

	/* CRAT */
	current = ALIGN(current, 8);
	crat = (struct acpi_crat_header *)current;
	acpi_create_crat(crat, acpi_fill_crat);
	current += crat->header.length;
	acpi_add_table(rsdp, crat);

	current = add_agesa_acpi_table(AMD_FSP_ACPI_ALIB_HOB_GUID, "ALIB", rsdp, current);

	/* IVRS */
	current = ALIGN(current, 8);
	ivrs = (acpi_ivrs_t *) current;
	acpi_create_ivrs(ivrs, acpi_fill_ivrs);
	current += ivrs->header.length;
	acpi_add_table(rsdp, ivrs);

	/* Add SRAT, MSCT, SLIT if needed in the future */

	return current;
}
