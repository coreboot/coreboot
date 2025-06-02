/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_DRAMC_PARAM_COMMON_H__
#define __SOC_MEDIATEK_DRAMC_PARAM_COMMON_H__

/*
 * NOTE: This file is shared between coreboot and dram blob. Any change in this
 * file should be synced to the other repository.
 */

#include <soc/dramc_soc.h>

enum DRAMC_PARAM_STATUS_CODES {
	DRAMC_SUCCESS = 0,
	DRAMC_ERR_INVALID_VERSION,
	DRAMC_ERR_INVALID_SIZE,
	DRAMC_ERR_INVALID_FLAGS,
	DRAMC_ERR_RECALIBRATE,
	DRAMC_ERR_INIT_DRAM,
	DRAMC_ERR_COMPLEX_RW_MEM_TEST,
	DRAMC_ERR_1ST_COMPLEX_RW_MEM_TEST,
	DRAMC_ERR_2ND_COMPLEX_RW_MEM_TEST,
	DRAMC_ERR_FAST_CALIBRATION,
};

enum DRAMC_PARAM_FLAG {
	DRAMC_FLAG_HAS_SAVED_DATA	= 0x0001,
};

enum DRAMC_PARAM_CONFIG {
	DRAMC_CONFIG_EMCP		= 0x0001,
	DRAMC_CONFIG_DVFS		= 0x0002,
	DRAMC_CONFIG_FAST_K		= 0x0004,
	/* Security configs */
	DRAMC_CONFIG_SCRAMBLE		= 0x0100,
};

struct dramc_param_header {
	u16 version;	/* DRAMC_PARAM_HEADER_VERSION, set in coreboot */
	u16 size;	/* size of whole dramc_param, set in coreboot */
	u16 status;	/* DRAMC_PARAM_STATUS_CODES, set in dram blob */
	u16 flags;	/* DRAMC_PARAM_FLAG, set in dram blob */
	u16 config;	/* DRAMC_PARAM_CONFIG, set in coreboot */
};

enum SDRAM_DDR_TYPE {
	DDR_TYPE_DISCRETE,
	DDR_TYPE_EMCP,
};

enum SDRAM_DDR_GEOMETRY_TYPE {
	DDR_TYPE_2CH_2RK_4GB_2_2,
	DDR_TYPE_2CH_2RK_6GB_3_3,
	DDR_TYPE_2CH_2RK_8GB_4_4_BYTE,
	DDR_TYPE_2CH_1RK_4GB_4_0,
	DDR_TYPE_2CH_2RK_6GB_2_4,
	DDR_TYPE_2CH_2RK_8GB_4_4,
};

struct sdram_info {
	u32 ddr_type;		/* SDRAM_DDR_TYPE */
	u32 ddr_geometry;	/* SDRAM_DDR_GEOMETRY_TYPE */
};

struct emi_mdl {
	u32 cona_val;
	u32 conh_val;
	u32 conf_val;
	u32 chn_cona_val;
};

struct ddr_mrr_info {
	u16 mr5_vendor_id;
	u16 mr6_revision_id;
	u16 mr7_revision_id;
	u64 mr8_density[RANK_MAX];
	u32 rank_nums;
	u8 die_num[RANK_MAX];
};

enum SDRAM_DVFS_FLAG {
	DRAMC_DISABLE_DVFS,
	DRAMC_ENABLE_DVFS,
};

enum SDRAM_VOLTAGE_TYPE {
	SDRAM_VOLTAGE_NVCORE_NVDRAM,
	SDRAM_VOLTAGE_HVCORE_HVDRAM,
	SDRAM_VOLTAGE_LVCORE_LVDRAM,
};

struct ddr_base_info {
	u16 config_dvfs;		/* SDRAM_DVFS_FLAG */
	u16 data_version;
	struct sdram_info sdram;
	u16 lpddr_type;
	u16 voltage_type;		/* SDRAM_VOLTAGE_TYPE */
	u32 support_ranks;
	u64 rank_size[RANK_MAX];
	struct emi_mdl emi_config;
	DRAM_CBT_MODE_T cbt_mode[RANK_MAX];
	struct ddr_mrr_info mrr_info;
	u32 data_rate;
};

const struct sdram_info *get_sdram_config(void);
struct dramc_param *get_dramc_param_from_blob(void *blob);
void dump_param_header(const void *blob);
int validate_dramc_param(const void *blob);
int is_valid_dramc_param(const void *blob);
int initialize_dramc_param(void *blob);

#endif
