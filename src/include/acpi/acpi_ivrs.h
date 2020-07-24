/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * AMD I/O Virtualization Technology (IOMMU)
 * Specification 48882-Rev 2.62-February 2015
 *
 * from http://www.uefi.org/acpi
 * I/O Virtualization Reporting Structure (IVRS)
 */

#ifndef __ACPI_ACPI_IVRS_H__
#define __ACPI_ACPI_IVRS_H__

#include <stdint.h>

/* I/O Virtualization Reporting Structure (IVRS) */
#define IVHD_BLOCK_TYPE_LEGACY__FIXED		0x10
#define IVHD_BLOCK_TYPE_FULL__FIXED		0x11
#define IVHD_BLOCK_TYPE_FULL__ACPI_HID		0x40

/* IVRS Revision Field */
#define IVRS_FORMAT_FIXED	0x01	/* Type 10h & 11h only */
#define IVRS_FORMAT_MIXED	0x02	/* Type 10h, 11h, & 40h */

/* IVRS IVinfo Field */
/* ATS response address range reserved */
#define IVINFO_HT_ATS_RESERVED			(1 << 22)

/* Virtual Address size - All other values are reserved */
#define IVINFO_VA_SIZE_32_BITS			(0x20 << 15)
#define IVINFO_VA_SIZE_40_BITS			(0x28 << 15)
#define IVINFO_VA_SIZE_48_BITS			(0x30 << 15)
#define IVINFO_VA_SIZE_64_BITS			(0x40 << 15)

/* Physical Address size - All other values are reserved */
#define IVINFO_PA_SIZE_40_BITS			(0x28 << 8)
#define IVINFO_PA_SIZE_48_BITS			(0x30 << 8)
#define IVINFO_PA_SIZE_52_BITS			(0x34 << 8)

/* Guest Virtual Address size  - All other values are reserved */
#define IVINFO_GVA_SIZE_48_BITS			(0x02 << 5)

/* Extended Feature Support */
#define IVINFO_EFR_SUPPORTED			0x01
#define EFR_FEATURE_SUP				(1 << 27)

/* IVHD Flags Field */
#define IVHD_FLAG_PPE_SUP			(1 << 7) /* Type 10h only */
#define IVHD_FLAG_PREF_SUP			(1 << 6) /* Type 10h only */
#define IVHD_FLAG_COHERENT			(1 << 5)
#define IVHD_FLAG_IOTLB_SUP			(1 << 4)
#define IVHD_FLAG_ISOC				(1 << 3)
#define IVHD_FLAG_RES_PASS_PW			(1 << 2)
#define IVHD_FLAG_PASS_PW			(1 << 1)
#define IVHD_FLAG_HT_TUN_EN			(1 << 0)

/* IVHD IOMMU Info Field */
#define IOMMU_INFO_UNIT_ID_SHIFT		8

/* IVHD IOMMU Feature Reporting Field */
#define IOMMU_FEATURE_HATS_SHIFT		30	 /* Type 10h only */
#define IOMMU_FEATURE_GATS_SHIFT		28	 /* Type 10h only */
#define IOMMU_FEATURE_MSI_NUM_PPR_SHIFT		23
#define IOMMU_FEATURE_PN_BANKS_SHIFT		17
#define IOMMU_FEATURE_PN_COUNTERS_SHIFT		13
#define IOMMU_FEATURE_PA_SMAX_SHIFT		8	 /* Type 10h only */
#define IOMMU_FEATURE_GLX_SHIFT			3

#define IOMMU_FEATURE_HE_SUP			(1 << 7) /* Type 10h only */
#define IOMMU_FEATURE_GA_SUP			(1 << 6) /* Type 10h only */
#define IOMMU_FEATURE_IA_SUP			(1 << 5) /* Type 10h only */
#define IOMMU_FEATURE_GLX_SINGLE_LEVEL		(0 << 3) /* Type 10h only */
#define IOMMU_FEATURE_GLX_TWO_LEVEL		(1 << 3) /* Type 10h only */
#define IOMMU_FEATURE_GLX_THREE_LEVEL		(2 << 3) /* Type 10h only */
#define IOMMU_FEATURE_GT_SUP			(1 << 2) /* Type 10h only */
#define IOMMU_FEATURE_NX_SUP			(1 << 1) /* Type 10h only */
#define IOMMU_FEATURE_XT_SUP			(1 << 0)

/* IVHD Device Entry Type Codes */
#define IVHD_DEV_4_BYTE_ALL			0x01
#define IVHD_DEV_4_BYTE_SELECT			0x02
#define IVHD_DEV_4_BYTE_START_RANGE		0x03
#define IVHD_DEV_4_BYTE_END_RANGE		0x04
#define IVHD_DEV_8_BYTE_ALIAS_SELECT		0x42
#define IVHD_DEV_8_BYTE_ALIAS_START_RANGE	0x43
#define IVHD_DEV_8_BYTE_EXT_SELECT		0x46
#define IVHD_DEV_8_BYTE_EXT_START_RANGE		0x47
#define IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV		0x48
#define IVHD_DEV_VARIABLE			0xF0

/* IVHD Device Table Entry (DTE) Settings */
#define IVHD_DTE_LINT_1_PASS			(1 << 7)
#define IVHD_DTE_LINT_0_PASS			(1 << 6)
#define IVHD_DTE_SYS_MGT_TGT_ABT		(0 << 4)
#define IVHD_DTE_SYS_MGT_NO_TRANS		(1 << 4)
#define IVHD_DTE_SYS_MGT_INTX_NO_TRANS		(2 << 4)
#define IVHD_DTE_SYS_MGT_TRANS			(3 << 4)
#define IVHD_DTE_NMI_PASS			(1 << 2)
#define IVHD_DTE_EXT_INT_PASS			(1 << 1)
#define IVHD_DTE_INIT_PASS			(1 << 0)

/* IVHD Device Entry Extended DTE Setting Field */
#define IVHD_DEV_EXT_ATS_DISABLE		(1 << 31)

/* IVHD Special Device Entry Variety Field */
#define IVHD_SPECIAL_DEV_IOAPIC			0x01
#define IVHD_SPECIAL_DEV_HPET			0x02

/* Device EntryType F0h UID Format */
#define IVHD_UID_NOT_PRESENT			0x00
#define IVHD_UID_INT				0x01
#define IVHD_UID_STRING				0x02

#define IOMMU_CAP_ID				0x0f

/* MMIO Offset 0x30: IOMMU Extended Feature Register */
#define MMIO_EXT_FEATURE_PRE_F_SUP_SHIFT	0
#define MMIO_EXT_FEATURE_PRE_F_SUP		(0x1 << MMIO_EXT_FEATURE_PRE_F_SUP_SHIFT)
#define MMIO_EXT_FEATURE_PPR_SUP_SHIFT		1
#define MMIO_EXT_FEATURE_PPR_SUP		(0x1 << MMIO_EXT_FEATURE_PPR_SUP_SHIFT)
#define MMIO_EXT_FEATURE_XT_SUP_SHIFT		2
#define MMIO_EXT_FEATURE_XT_SUP			(0x1 << MMIO_EXT_FEATURE_XT_SUP_SHIFT)
#define MMIO_EXT_FEATURE_NX_SUP_SHIFT		3
#define MMIO_EXT_FEATURE_NX_SUP			(0x1 << MMIO_EXT_FEATURE_NX_SUP_SHIFT)
#define MMIO_EXT_FEATURE_GT_SUP_SHIFT		4
#define MMIO_EXT_FEATURE_GT_SUP			(0x1 << MMIO_EXT_FEATURE_GT_SUP_SHIFT)
#define MMIO_EXT_FEATURE_IA_SUP_SHIFT		6
#define MMIO_EXT_FEATURE_IA_SUP			(0x1 << MMIO_EXT_FEATURE_IA_SUP_SHIFT)
#define MMIO_EXT_FEATURE_GA_SUP_SHIFT		7
#define MMIO_EXT_FEATURE_GA_SUP			(0x1 << MMIO_EXT_FEATURE_GA_SUP_SHIFT)
#define MMIO_EXT_FEATURE_HE_SUP_SHIFT		8
#define MMIO_EXT_FEATURE_HE_SUP			(0x1 << MMIO_EXT_FEATURE_HE_SUP_SHIFT)
#define MMIO_EXT_FEATURE_PC_SUP_SHIFT		9
#define MMIO_EXT_FEATURE_PC_SUP			(0x1 << MMIO_EXT_FEATURE_PC_SUP_SHIFT)
#define MMIO_EXT_FEATURE_HATS_SHIFT		10
#define MMIO_EXT_FEATURE_HATS_MASK		(0x3 << MMIO_EXT_FEATURE_HATS_SHIFT)
#define MMIO_EXT_FEATURE_GATS_SHIFT		12
#define MMIO_EXT_FEATURE_GATS_MASK		(0x3 << MMIO_EXT_FEATURE_GATS_SHIFT)
#define MMIO_EXT_FEATURE_GLX_SHIFT		14
#define MMIO_EXT_FEATURE_GLX_SUP_MASK		(0x3 << MMIO_EXT_FEATURE_GLX_SHIFT)
#define MMIO_EXT_FEATURE_SMI_F_SUP_SHIFT	16
#define MMIO_EXT_FEATURE_SMI_F_SUP_MASK		(0x3 << MMIO_EXT_FEATURE_SMI_F_SUP_SHIFT)
#define MMIO_EXT_FEATURE_SMI_FRC_SHIFT		18
#define MMIO_EXT_FEATURE_SMI_FRC_MASK		(0x7 << MMIO_EXT_FEATURE_SMI_FRC_SHIFT)
#define MMIO_EXT_FEATURE_GAM_SUP_SHIFT		21
#define MMIO_EXT_FEATURE_GAM_SUP_MASK		(0x7 << MMIO_EXT_FEATURE_GAM_SUP_SHIFT)
#define MMIO_EXT_FEATURE_PAS_MAX_SHIFT		32
#define MMIO_EXT_FEATURE_PAS_MAX_MASK		(0x1fULL << MMIO_EXT_FEATURE_PAS_MAX_SHIFT)

/* MMIO Offset 0x18: IOMMU Control Register */
#define MMIO_CTRL_IOMMU_EN			(1 << 0)
#define MMIO_CTRL_HT_TUN_EN			(1 << 1)
#define MMIO_CTRL_PASS_PW			(1 << 8)
#define MMIO_CTRL_RES_PASS_PW			(1 << 9)
#define MMIO_CTRL_COHERENT			(1 << 10)
#define MMIO_CTRL_ISOC				(1 << 11)

/* MMIO Offset 0x4000: Counter Configuration Register */
#define MMIO_CNT_CFG_N_CNT_BANKS_SHIFT		12
#define MMIO_CNT_CFG_N_COUNTER_BANKS		(0x3f << MMIO_CNT_CFG_N_CNT_BANKS_SHIFT)
#define MMIO_CNT_CFG_N_COUNTER_SHIFT		7
#define MMIO_CNT_CFG_N_COUNTER			(0xf << MMIO_CNT_CFG_N_COUNTER_SHIFT)

/* Capability offset 0 */
#define CAP_OFFSET_0_IOTLB_SP_SHIFT		24
#define CAP_OFFSET_0_IOTLB_SP			(1 << CAP_OFFSET_0_IOTLB_SP_SHIFT)

/// Capability offset 10h
#define CAP_OFFSET_10_MSI_NUM_PPR_SHIFT		27
#define CAP_OFFSET_10_MSI_NUM_PPR		(0x1f << CAP_OFFSET_10_MSI_NUM_PPR_SHIFT)

/* IVHD (I/O Virtualization Hardware Definition Block) 4-byte entry */
typedef struct ivrs_ivhd_generic {
	uint8_t type;
	uint16_t dev_id;
	uint8_t dte_setting;
} __packed ivrs_ivhd_generic_t;

/* IVHD (I/O Virtualization Hardware Definition Block) 8-byte entries */
typedef struct ivrs_ivhd_alias {
	uint8_t type;
	uint16_t dev_id;
	uint8_t dte_setting;
	uint8_t reserved1;
	uint16_t source_dev_id;
	uint8_t reserved2;
} __packed ivrs_ivhd_alias_t;

/* IVRS IVHD (I/O Virtualization Hardware Definition Block) Type 40h */
typedef struct acpi_ivrs_ivhd_40 {
	uint8_t type;
	uint8_t flags;
	uint16_t length;
	uint16_t device_id;
	uint16_t capability_offset;
	uint32_t iommu_base_low;
	uint32_t iommu_base_high;
	uint16_t pci_segment_group;
	uint16_t iommu_info;
	uint32_t iommu_attributes;
	uint32_t efr_reg_image_low;
	uint32_t efr_reg_image_high;
	uint32_t reserved[2];
	uint8_t entry[0];
} __packed acpi_ivrs_ivhd40_t;

typedef struct ivrs_ivhd_extended {
	uint8_t type;
	uint16_t dev_id;
	uint8_t dte_setting;
	uint32_t extended_dte_setting;
} __packed ivrs_ivhd_extended_t;

typedef struct ivrs_ivhd_special {
	uint8_t type;
	uint16_t reserved;
	uint8_t dte_setting;
	uint8_t handle;
	uint16_t source_dev_id;
	uint8_t variety;
} __packed ivrs_ivhd_special_t;

typedef struct ivrs_ivhd_f0_entry {
	uint8_t type;
	uint16_t dev_id;
	uint8_t dte_setting;
	uint8_t hardware_id[8];
	uint8_t compatible_id[8];
	uint8_t uuid_format;
	uint8_t uuid_length;
} __packed ivrs_ivhd_f0_entry_t;

#endif /* __ACPI_ACPI_IVRS_H__ */
