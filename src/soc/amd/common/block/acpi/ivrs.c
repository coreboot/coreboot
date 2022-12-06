/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_ivrs.h>
#include <amdblocks/acpi.h>
#include <amdblocks/cpu.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/ioapic.h>
#include <arch/ioapic.h>
#include <arch/mmio.h>
#include <console/console.h>
#include <cpu/amd/cpuid.h>
#include <cpu/amd/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/acpi.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

#define MAX_DEV_ID 0xFFFF

unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current)
{
	ivrs_ivhd_special_t *ivhd_ioapic = (ivrs_ivhd_special_t *)current;
	memset(ivhd_ioapic, 0, sizeof(*ivhd_ioapic));

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->dte_setting = IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
				   IVHD_DTE_SYS_MGT_NO_TRANS | IVHD_DTE_NMI_PASS |
				   IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS;
	ivhd_ioapic->handle = get_ioapic_id(VIO_APIC_VADDR);
	ivhd_ioapic->source_dev_id = PCI_DEVFN(SMBUS_DEV, SMBUS_FUNC);
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	ivhd_ioapic = (ivrs_ivhd_special_t *)current;
	memset(ivhd_ioapic, 0, sizeof(*ivhd_ioapic));

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->handle = get_ioapic_id((u8 *)GNB_IO_APIC_ADDR);
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
	ivrs_ivhd_f0_entry_t *ivhd_f0 = (ivrs_ivhd_f0_entry_t *)current;

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
		ivhd_40->efr_reg_image_low  = read32p(ivhd_40->iommu_base_low + 0x30);
		ivhd_40->efr_reg_image_high = read32p(ivhd_40->iommu_base_low + 0x34);
	}

	current += sizeof(acpi_ivrs_ivhd40_t);

	/* Now repeat all the device entries from type 10h */
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), MAX_DEV_ID, 0);
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
	ivhd11_attr_ptr = (ivhd11_iommu_attr_t *)&ivrs->ivhd.iommu_feature_info;
	ivhd_11->iommu_attributes.perf_counters = ivhd11_attr_ptr->perf_counters;
	ivhd_11->iommu_attributes.perf_counter_banks = ivhd11_attr_ptr->perf_counter_banks;
	ivhd_11->iommu_attributes.msi_num_ppr = ivhd11_attr_ptr->msi_num_ppr;

	if (pci_read_config32(iommu_dev, ivhd_11->capability_offset) & EFR_FEATURE_SUP) {
		ivhd_11->efr_reg_image_low  = read32p(ivhd_11->iommu_base_low + 0x30);
		ivhd_11->efr_reg_image_high = read32p(ivhd_11->iommu_base_low + 0x34);
	}

	current += sizeof(acpi_ivrs_ivhd11_t);

	/* Now repeat all the device entries from type 10h */
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), MAX_DEV_ID, 0);
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

unsigned long acpi_fill_ivrs(acpi_ivrs_t *ivrs, unsigned long current)
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
		mmio_x18_value = read64p(ivrs->ivhd.iommu_base_low + 0x18);
		mmio_x30_value = read64p(ivrs->ivhd.iommu_base_low + 0x30);
		mmio_x4000_value = read64p(ivrs->ivhd.iommu_base_low + 0x4000);

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
	 * Add all possible PCI devices that can generate transactions
	 * processed by IOMMU. Start with device 00:01.0
	*/
	current_backup = current;
	current = ivhd_dev_range(current, PCI_DEVFN(1, 0), MAX_DEV_ID, 0);
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
