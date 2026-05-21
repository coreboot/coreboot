/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_QUALCOMM_X1P42100_PLATFORM_INFO_H__
#define __SOC_QUALCOMM_X1P42100_PLATFORM_INFO_H__

/* ADSP BOOT_PARAMS helpers */

/* BOOT_PARAMS bitfield definitions */
#define CHIP_FAMILY_MASK			0xFFFF
#define CHIP_FAMILY_SHIFT			16
#define CHIP_ID_MASK				0xFFFF
#define CHIP_VERSION_MAJOR_SHIFT		8
#define CHIP_VERSION_MASK			0xFF
#define PLATFORM_FIELD_MASK			0xFF
#define PLATFORM_SUBTYPE_SHIFT			16
#define PLATFORM_TYPE_SHIFT			24
#define PLATFORM_VERSION_MAJOR_SHIFT		8

#define EVB_ENABLE				1

/*
 * ADSP BOOT_PARAMS chipinfo identifiers.
 *
 * Keep values as macros (no literals in packer code) to match SMEM chipinfo:
 * - CHIPINFO_FAMILY_*: eChipInfoFamily
 * - CHIPINFO_ID_SCP_*: eChipInfoId (SCP)
 */

/* Hamoa (SC8380XP) identifiers used by ADSP BOOT_PARAMS on x1p42100 platforms. */
#define HAMOA_FAMILY				0x0088
#define HAMOA_ID_SCP				0x022B

/* Purwa Compute (SC8340XP / X1P4x100) */
#define X1P42100_FAMILY				0x009A
#define X1P42100_ID_SCP				0x027B

#define CHIPINFO_CHIP_VERSION			0x00020000 /* nChipVersion (SMEM) */
#define CHIPINFO_CHIP_VERSION_MAJOR		0x02
#define CHIPINFO_CHIP_VERSION_MINOR		0x00

/* x1p42100 platform info used for BOOT_PARAMS[4] packing. */
#define PLATFORMINFO_TYPE			0x28 /* ePlatformType */
#define PLATFORMINFO_SUBTYPE			0x00 /* nPlatformSubtype */
#define PLATFORMINFO_VERSION			0x00010000 /* nPlatformVersion */
#define PLATFORMINFO_VERSION_MAJOR		0x01
#define PLATFORMINFO_VERSION_MINOR		0x00

#define TCSR_SOC_HW_VERSION_DEVICE_NUM_HAMOA    0x09
#define TCSR_SOC_HW_VERSION_DEVICE_NUM_X1P42100 0x16

enum qclib_soc_id {
	SOC_ID_UNKNOWN,
	SOC_ID_HAMOA,
	SOC_ID_X1P42100,
};

union tcsr_soc_hw_version {
	struct {
		uint32_t minor_version  : 8;  /* bits[7:0]   */
		uint32_t major_version  : 8;  /* bits[15:8]  */
		uint32_t device_number  : 12; /* bits[27:16] */
		uint32_t family_number  : 4;  /* bits[31:28] */
	};
	uint32_t data;
};

enum qclib_soc_id platform_get_soc_id(void);

#endif /* __SOC_QUALCOMM_X1P42100_PLATFORM_INFO_H__ */
