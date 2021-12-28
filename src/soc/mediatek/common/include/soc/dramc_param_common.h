/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_DRAMC_PARAM_COMMON_H__
#define __SOC_MEDIATEK_DRAMC_PARAM_COMMON_H__

/*
 * NOTE: This file is shared between coreboot and dram blob. Any change in this
 * file should be synced to the other repository.
 */

enum DRAMC_PARAM_FLAG {
	DRAMC_FLAG_HAS_SAVED_DATA	= 0x0001,
};

enum DRAMC_PARAM_CONFIG {
	DRAMC_CONFIG_EMCP		= 0x0001,
	DRAMC_CONFIG_DVFS		= 0x0002,
	DRAMC_CONFIG_FAST_K		= 0x0004,
};

struct dramc_param_header {
	u16 version;	/* DRAMC_PARAM_HEADER_VERSION, set in coreboot */
	u16 size;	/* size of whole dramc_param, set in coreboot */
	u16 status;	/* DRAMC_PARAM_STATUS_CODES, set in dram blob */
	u16 flags;	/* DRAMC_PARAM_FLAG, set in dram blob */
	u16 config;	/* DRAMC_PARAM_CONFIG, set in coreboot */
};

#endif
