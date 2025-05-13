/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_COMMON_STORAGE_H
#define SOC_MEDIATEK_COMMON_STORAGE_H

/* Bits [31:28] for base type (e.g. UFS, NVMe). */
#define _BASE_TYPE_SHIFT 28
#define _BASE_TYPE_MASK (0xf << _BASE_TYPE_SHIFT)
#define _BASE_TYPE(x) (((x) & 0xf) << _BASE_TYPE_SHIFT)

#define _BASE_TYPE_UFS _BASE_TYPE(0x1)
#define _BASE_TYPE_NVME _BASE_TYPE(0x2)
#define _BASE_TYPE_EMMC _BASE_TYPE(0x3)

enum mtk_storage_type {
	STORAGE_UNKNOWN		= 0,
	STORAGE_UFS_22		= _BASE_TYPE_UFS | 0x220,
	STORAGE_UFS_31		= _BASE_TYPE_UFS | 0x310,
	STORAGE_UFS_40		= _BASE_TYPE_UFS | 0x400,
	STORAGE_UFS_40_HS	= _BASE_TYPE_UFS | 0x401,
	STORAGE_NVME		= _BASE_TYPE_NVME,
	STORAGE_EMMC		= _BASE_TYPE_EMMC,
};

enum mtk_storage_type mainboard_get_storage_type(void);

#endif /* SOC_MEDIATEK_COMMON_STORAGE_H */
