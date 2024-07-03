/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_ivrs.h>
#include <amdblocks/acpi.h>
#include <amdblocks/cpu.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/iommu.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/amd/cpuid.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

static unsigned long acpi_fill_ivrs_ioapic(unsigned long current, uintptr_t ioapic_base,
					    uint16_t src_devid, uint8_t dte_setting)
{
	ivrs_ivhd_special_t *ivhd_ioapic = (ivrs_ivhd_special_t *)current;
	memset(ivhd_ioapic, 0, sizeof(*ivhd_ioapic));

	ivhd_ioapic->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_ioapic->dte_setting = dte_setting;
	ivhd_ioapic->handle = get_ioapic_id(ioapic_base);
	ivhd_ioapic->source_dev_id = src_devid;
	ivhd_ioapic->variety = IVHD_SPECIAL_DEV_IOAPIC;
	current += sizeof(ivrs_ivhd_special_t);

	return current;
}

static unsigned long ivhd_describe_hpet(unsigned long current, uint8_t hndl, uint16_t src_devid)
{
	ivrs_ivhd_special_t *ivhd_hpet = (ivrs_ivhd_special_t *)current;
	memset(ivhd_hpet, 0, sizeof(*ivhd_hpet));

	ivhd_hpet->type = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;
	ivhd_hpet->handle = hndl;
	ivhd_hpet->source_dev_id = src_devid; /* function 0 of FCH PCI device */
	ivhd_hpet->variety = IVHD_SPECIAL_DEV_HPET;
	current += sizeof(ivrs_ivhd_special_t);

	return current;
}

static unsigned long ivhd_describe_f0_device(unsigned long current, uint16_t dev_id,
					     const char acpi_hid[8], uint8_t datasetting)
{
	ivrs_ivhd_f0_entry_t *ivhd_f0 = (ivrs_ivhd_f0_entry_t *)current;
	memset(ivhd_f0, 0, sizeof(*ivhd_f0));

	ivhd_f0->type = IVHD_DEV_VARIABLE;
	ivhd_f0->dev_id = dev_id;
	ivhd_f0->dte_setting = datasetting;

	memcpy(ivhd_f0->hardware_id, acpi_hid, sizeof(ivhd_f0->hardware_id));

	current += sizeof(ivrs_ivhd_f0_entry_t);
	return current;
}

static unsigned long ivhd_dev_range(unsigned long current, uint16_t start_devid,
				    uint16_t end_devid, uint8_t setting)
{
	/* 4-byte IVHD structures must be aligned to the 4-byte boundary. */
	current = ALIGN_UP(current, 4);
	ivrs_ivhd_generic_t *ivhd_range = (ivrs_ivhd_generic_t *)current;
	memset(ivhd_range, 0, sizeof(*ivhd_range));

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
		memset(ivhd_entry, 0, sizeof(*ivhd_entry));

		ivhd_entry->type = type;
		ivhd_entry->dev_id = dev->path.pci.devfn | (dev->upstream->secondary << 8);
		ivhd_entry->dte_setting = data;
		*current += sizeof(ivrs_ivhd_generic_t);
	} else if (type == IVHD_DEV_8_BYTE_ALIAS_SELECT) {
		ivrs_ivhd_alias_t *ivhd_entry = (ivrs_ivhd_alias_t *)*current;
		memset(ivhd_entry, 0, sizeof(*ivhd_entry));

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
				      unsigned long *current)
{
	unsigned int header_type, is_pcie;

	header_type = dev->hdr_type & 0x7f;
	is_pcie = pci_find_capability(dev, PCI_CAP_ID_PCIE);

	if (((header_type == PCI_HEADER_TYPE_NORMAL) ||
	     (header_type == PCI_HEADER_TYPE_BRIDGE)) && is_pcie) {
		/* Device or Bridge is PCIe */
		add_ivhd_dev_entry(parent, dev, current, IVHD_DEV_4_BYTE_SELECT, 0x0);
	} else if ((header_type == PCI_HEADER_TYPE_NORMAL) && !is_pcie) {
		/* Device is legacy PCI or PCI-X */
		add_ivhd_dev_entry(parent, dev, current, IVHD_DEV_8_BYTE_ALIAS_SELECT, 0x0);
	}
}

static void add_ivhd_device_entries(struct device *parent, struct device *dev,
				    unsigned int depth, int linknum, int8_t *root_level,
				    unsigned long *current, uint16_t nb_bus)
{
	struct device *sibling;

	if (!root_level)
		return;

	if (dev->path.type == DEVICE_PATH_PCI) {
		if ((dev->upstream->secondary == nb_bus) &&
		    (dev->path.pci.devfn == 0x0))
			*root_level = depth;

		if ((*root_level != -1) && (dev->enabled))
			if (depth != *root_level)
				ivrs_add_device_or_bridge(parent, dev, current);
	}

	if (!dev->downstream)
		return;
	for (sibling = dev->downstream->children; sibling; sibling = sibling->sibling)
		add_ivhd_device_entries(dev, sibling, depth + 1, depth, root_level, current,
					nb_bus);
}

static unsigned long acpi_ivhd_misc(unsigned long current, struct device *dev)
{
	u8 dte_setting = IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
		       IVHD_DTE_SYS_MGT_NO_TRANS | IVHD_DTE_NMI_PASS |
		       IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS;
	int8_t root_level = -1;
	struct resource *res;

	/*
	 * Add all possible PCI devices in the domain that can generate transactions
	 * processed by IOMMU. Start with device <bus>:01.0
	*/
	current = ivhd_dev_range(current, PCI_DEVFN(0, 3) | (dev->downstream->secondary << 8),
				 0xff | (dev->downstream->subordinate << 8), 0);

	add_ivhd_device_entries(NULL, dev, 0, -1, &root_level,
		&current, dev->downstream->secondary);

	res = probe_resource(dev, IOMMU_IOAPIC_IDX);
	if (res) {
		/* Describe IOAPIC associated with the IOMMU */
		current = acpi_fill_ivrs_ioapic(current, (uintptr_t)res->base,
				      PCI_DEVFN(0, 1) | (dev->downstream->secondary << 8), 0);
	}

	/* If the domain has secondary bus as zero then associate HPET & FCH IOAPIC */
	if (dev->downstream->secondary == 0) {
		/* Describe HPET */
		current = ivhd_describe_hpet(current, 0x00, SMBUS_DEVFN);
		/* Describe FCH IOAPICs */
		current = acpi_fill_ivrs_ioapic(current, IO_APIC_ADDR,
				      SMBUS_DEVFN, dte_setting);
	}

	return current;
}

static unsigned long acpi_fill_ivrs40(unsigned long current, acpi_ivrs_ivhd_t *ivhd,
				       struct device *nb_dev, struct device *iommu_dev)
{
	acpi_ivrs_ivhd40_t *ivhd_40 = (acpi_ivrs_ivhd40_t *)current;
	unsigned long current_backup;

	memset(ivhd_40, 0, sizeof(*ivhd_40));

	/* Enable EFR */
	ivhd_40->type = IVHD_BLOCK_TYPE_FULL__ACPI_HID;
	/* For type 40h bits 6 and 7 are reserved */
	ivhd_40->flags = ivhd->flags & 0x3f;
	ivhd_40->length = sizeof(struct acpi_ivrs_ivhd_40);
	/* BDF <bus>:00.2 */
	ivhd_40->device_id = 0x02 | (nb_dev->upstream->secondary << 8);
	ivhd_40->capability_offset = pci_find_capability(iommu_dev, IOMMU_CAP_ID);
	ivhd_40->iommu_base_low = ivhd->iommu_base_low;
	ivhd_40->iommu_base_high = ivhd->iommu_base_high;
	ivhd_40->pci_segment_group = nb_dev->upstream->segment_group;
	ivhd_40->iommu_info = ivhd->iommu_info;
	/* For type 40h bits 31:28 and 12:0 are reserved */
	ivhd_40->iommu_attributes = ivhd->iommu_feature_info & 0xfffe000;

	if (pci_read_config32(iommu_dev, ivhd_40->capability_offset) & EFR_FEATURE_SUP) {
		ivhd_40->efr_reg_image_low  = read32p(ivhd_40->iommu_base_low + 0x30);
		ivhd_40->efr_reg_image_high = read32p(ivhd_40->iommu_base_low + 0x34);
	}

	current += sizeof(acpi_ivrs_ivhd40_t);

	/* Now repeat all the device entries from type 10h */
	current_backup = current;
	current = acpi_ivhd_misc(current, nb_dev->upstream->dev);

	if (nb_dev->upstream->secondary == 0) {
		/* Describe EMMC */
		if (CONFIG(SOC_AMD_COMMON_BLOCK_EMMC)) {
			/* PCI_DEVFN(0x13, 1) doesn't exist in the hardware, but it's what the
			 * reference code uses. Maybe to have a unique PCI device to put into
			 * the field that doesn't collide with any existing device? */
			current = ivhd_describe_f0_device(current, PCI_DEVFN(0x13, 1),
						"AMDI0040",
						IVHD_DTE_LINT_1_PASS | IVHD_DTE_LINT_0_PASS |
						IVHD_DTE_SYS_MGT_TRANS   | IVHD_DTE_NMI_PASS |
						IVHD_DTE_EXT_INT_PASS | IVHD_DTE_INIT_PASS);
		}
	}
	ivhd_40->length += (current - current_backup);

	return current;
}

static unsigned long acpi_fill_ivrs11(unsigned long current, acpi_ivrs_ivhd_t *ivhd,
				       struct device *nb_dev, struct device *iommu_dev)
{
	acpi_ivrs_ivhd11_t *ivhd_11 = (acpi_ivrs_ivhd11_t *)current;
	ivhd11_iommu_attr_t *ivhd11_attr_ptr;
	unsigned long current_backup;

	/*
	 * In order to utilize all features, firmware should expose type 11h
	 * IVHD which supersedes the type 10h.
	 */
	memset(ivhd_11, 0, sizeof(*ivhd_11));

	/* Enable EFR */
	ivhd_11->type = IVHD_BLOCK_TYPE_FULL__FIXED;
	/* For type 11h bits 6 and 7 are reserved */
	ivhd_11->flags = ivhd->flags & 0x3f;
	ivhd_11->length = sizeof(struct acpi_ivrs_ivhd_11);
	/* BDF <bus>:00.2 */
	ivhd_11->device_id = 0x02 | (nb_dev->upstream->secondary << 8);
	ivhd_11->capability_offset = pci_find_capability(iommu_dev, IOMMU_CAP_ID);
	ivhd_11->iommu_base_low = ivhd->iommu_base_low;
	ivhd_11->iommu_base_high = ivhd->iommu_base_high;
	ivhd_11->pci_segment_group = nb_dev->upstream->segment_group;
	ivhd_11->iommu_info = ivhd->iommu_info;
	ivhd11_attr_ptr = (ivhd11_iommu_attr_t *)&ivhd->iommu_feature_info;
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
	current = acpi_ivhd_misc(current, nb_dev->upstream->dev);
	ivhd_11->length += (current - current_backup);

	return acpi_fill_ivrs40(current, ivhd, nb_dev, iommu_dev);
}

static unsigned long acpi_fill_ivrs(acpi_ivrs_t *ivrs, unsigned long current)
{
	unsigned long current_backup;
	uint64_t mmio_x30_value;
	uint64_t mmio_x18_value;
	uint64_t mmio_x4000_value;
	uint32_t cap_offset_0;
	uint32_t cap_offset_10;
	struct acpi_ivrs_ivhd *ivhd;
	struct device *iommu_dev;
	struct device *nb_dev;
	struct device *dev = NULL;

	if (ivrs == NULL) {
		printk(BIOS_WARNING, "%s: ivrs is NULL\n", __func__);
		return current;
	}

	ivhd = &ivrs->ivhd;

	while ((dev = dev_find_path(dev, DEVICE_PATH_DOMAIN)) != NULL) {
		nb_dev = pcidev_path_behind(dev->downstream, PCI_DEVFN(0, 0));
		iommu_dev = pcidev_path_behind(dev->downstream, PCI_DEVFN(0, 2));
		if (!nb_dev) {
			printk(BIOS_WARNING, "%s: Northbridge device not present!\n", __func__);
			printk(BIOS_WARNING, "%s: IVRS table not generated...\n", __func__);
			return (unsigned long)ivrs;
		}

		if (!iommu_dev) {
			printk(BIOS_WARNING, "%s: IOMMU device not found\n", __func__);
			return (unsigned long)ivrs;
		}

		ivhd->type = IVHD_BLOCK_TYPE_LEGACY__FIXED;
		ivhd->length = sizeof(struct acpi_ivrs_ivhd);

		/* BDF <bus>:00.2 */
		ivhd->device_id = PCI_DEVFN(0, 2) | (nb_dev->upstream->secondary << 8);
		ivhd->capability_offset = pci_find_capability(iommu_dev, IOMMU_CAP_ID);
		ivhd->iommu_base_low = pci_read_config32(iommu_dev, IOMMU_CAP_BASE_LO) & 0xffffc000;
		ivhd->iommu_base_high = pci_read_config32(iommu_dev, IOMMU_CAP_BASE_HI);

		cap_offset_0 = pci_read_config32(iommu_dev, ivhd->capability_offset);
		cap_offset_10 = pci_read_config32(iommu_dev,
						ivhd->capability_offset + 0x10);
		mmio_x18_value = read64p(ivhd->iommu_base_low + 0x18);
		mmio_x30_value = read64p(ivhd->iommu_base_low + 0x30);
		mmio_x4000_value = read64p(ivhd->iommu_base_low + 0x4000);

		ivhd->flags |= ((mmio_x30_value & MMIO_EXT_FEATURE_PPR_SUP) ?
							IVHD_FLAG_PPE_SUP : 0);
		ivhd->flags |= ((mmio_x30_value & MMIO_EXT_FEATURE_PRE_F_SUP) ?
							IVHD_FLAG_PREF_SUP : 0);
		ivhd->flags |= ((mmio_x18_value & MMIO_CTRL_COHERENT) ?
							IVHD_FLAG_COHERENT : 0);
		ivhd->flags |= ((cap_offset_0 & CAP_OFFSET_0_IOTLB_SP) ?
							IVHD_FLAG_IOTLB_SUP : 0);
		ivhd->flags |= ((mmio_x18_value & MMIO_CTRL_ISOC) ?
							IVHD_FLAG_ISOC : 0);
		ivhd->flags |= ((mmio_x18_value & MMIO_CTRL_RES_PASS_PW) ?
							IVHD_FLAG_RES_PASS_PW : 0);
		ivhd->flags |= ((mmio_x18_value & MMIO_CTRL_PASS_PW) ?
							IVHD_FLAG_PASS_PW : 0);
		ivhd->flags |= ((mmio_x18_value & MMIO_CTRL_HT_TUN_EN) ?
							IVHD_FLAG_HT_TUN_EN : 0);

		ivhd->pci_segment_group = nb_dev->upstream->segment_group;

		ivhd->iommu_info = pci_read_config16(iommu_dev,
			ivhd->capability_offset + 0x10) & 0x1F;
		ivhd->iommu_info |= (pci_read_config16(iommu_dev,
			ivhd->capability_offset + 0xC) & 0x1F) << IOMMU_INFO_UNIT_ID_SHIFT;

		ivhd->iommu_feature_info = 0;
		ivhd->iommu_feature_info |= (mmio_x30_value & MMIO_EXT_FEATURE_HATS_MASK)
			<< (IOMMU_FEATURE_HATS_SHIFT - MMIO_EXT_FEATURE_HATS_SHIFT);

		ivhd->iommu_feature_info |= (mmio_x30_value & MMIO_EXT_FEATURE_GATS_MASK)
			<< (IOMMU_FEATURE_GATS_SHIFT - MMIO_EXT_FEATURE_GATS_SHIFT);

		ivhd->iommu_feature_info |= (cap_offset_10 & CAP_OFFSET_10_MSI_NUM_PPR)
			>> (CAP_OFFSET_10_MSI_NUM_PPR_SHIFT
				- IOMMU_FEATURE_MSI_NUM_PPR_SHIFT);

		ivhd->iommu_feature_info |= (mmio_x4000_value &
			MMIO_CNT_CFG_N_COUNTER_BANKS)
			<< (IOMMU_FEATURE_PN_BANKS_SHIFT - MMIO_CNT_CFG_N_CNT_BANKS_SHIFT);

		ivhd->iommu_feature_info |= (mmio_x4000_value & MMIO_CNT_CFG_N_COUNTER)
			<< (IOMMU_FEATURE_PN_COUNTERS_SHIFT - MMIO_CNT_CFG_N_COUNTER_SHIFT);
		ivhd->iommu_feature_info |= (mmio_x30_value &
			MMIO_EXT_FEATURE_PAS_MAX_MASK)
			>> (MMIO_EXT_FEATURE_PAS_MAX_SHIFT - IOMMU_FEATURE_PA_SMAX_SHIFT);
		ivhd->iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_HE_SUP)
			? IOMMU_FEATURE_HE_SUP : 0);
		ivhd->iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_GA_SUP)
			? IOMMU_FEATURE_GA_SUP : 0);
		ivhd->iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_IA_SUP)
			? IOMMU_FEATURE_IA_SUP : 0);
		ivhd->iommu_feature_info |= (mmio_x30_value &
			MMIO_EXT_FEATURE_GLX_SUP_MASK)
			>> (MMIO_EXT_FEATURE_GLX_SHIFT - IOMMU_FEATURE_GLX_SHIFT);
		ivhd->iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_GT_SUP)
			? IOMMU_FEATURE_GT_SUP : 0);
		ivhd->iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_NX_SUP)
			? IOMMU_FEATURE_NX_SUP : 0);
		ivhd->iommu_feature_info |= ((mmio_x30_value & MMIO_EXT_FEATURE_XT_SUP)
			? IOMMU_FEATURE_XT_SUP : 0);

		/* Enable EFR if supported */
		ivrs->iv_info = pci_read_config32(iommu_dev,
					ivhd->capability_offset + 0x10) & 0x007fffe0;
		if (pci_read_config32(iommu_dev,
					ivhd->capability_offset) & EFR_FEATURE_SUP)
			ivrs->iv_info |= IVINFO_EFR_SUPPORTED;


		current_backup = current;
		current = acpi_ivhd_misc(current, dev);
		ivhd->length += (current - current_backup);

		/* If EFR is not supported, IVHD type 11h is reserved */
		if (!(ivrs->iv_info & IVINFO_EFR_SUPPORTED))
			return current;

		current = acpi_fill_ivrs11(current, ivhd, nb_dev, iommu_dev);

		ivhd = (struct acpi_ivrs_ivhd *)current;
		current += sizeof(struct acpi_ivrs_ivhd);
	}
	current -= sizeof(struct acpi_ivrs_ivhd);

	return current;
}

unsigned long acpi_add_ivrs_table(unsigned long current, acpi_rsdp_t *rsdp)
{
	acpi_ivrs_t *ivrs;

	current = acpi_align_current(current);
	ivrs = (acpi_ivrs_t *)current;
	acpi_create_ivrs(ivrs, acpi_fill_ivrs);
	current += ivrs->header.length;
	acpi_add_table(rsdp, ivrs);

	return current;
}
