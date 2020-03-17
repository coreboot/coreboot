/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_MEDIATEK_MT8183_DRAMC_PARAM_H
#define SOC_MEDIATEK_MT8183_DRAMC_PARAM_H

#include <stdint.h>
#include <sys/types.h>

#include "emi.h"

enum {
	DRAMC_PARAM_HEADER_MAGIC = 0x44524d4b,
	DRAMC_PARAM_HEADER_VERSION = 2,
};

enum DRAMC_PARAM_STATUS_CODES {
	DRAMC_SUCCESS = 0,
	DRAMC_ERR_INVALID_MAGIC,
	DRAMC_ERR_INVALID_VERSION,
	DRAMC_ERR_INVALID_SIZE,
	DRAMC_ERR_INVALID_CHECKSUM,
	DRAMC_ERR_INVALID_FLAGS,
	DRAMC_ERR_RECALIBRATE,
	DRAMC_ERR_INIT_DRAM,
	DRAMC_ERR_COMPLEX_RW_MEM_TEST,
	DRAMC_ERR_1ST_COMPLEX_RW_MEM_TEST,
	DRAMC_ERR_2ND_COMPLEX_RW_MEM_TEST,
};

/* Bit flags */
enum DRAMC_PARAM_CONFIG {
	DRAMC_CONFIG_EMCP = 0x0001,
	DRAMC_CONFIG_DVFS = 0x0002,
};

enum DRAMC_PARAM_FLAGS {
	DRAMC_FLAG_HAS_SAVED_DATA = 0x0001,
};

struct dramc_param_header {
	u32 status;	/* DRAMC_PARAM_STATUS_CODES */
	u32 magic;
	u32 version;
	u32 size;	/* size of whole dramc_param */
	u16 config;	/* DRAMC_PARAM_CONFIG */
	u16 flags;	/* DRAMC_PARAM_FLAGS */
	u32 checksum;
};

struct dramc_param {
	struct dramc_param_header header;
	void (*do_putc)(unsigned char c);
	struct sdram_params freq_params[DRAM_DFS_SHUFFLE_MAX];
};

struct dramc_param_ops {
	struct dramc_param *param;
	bool (*read_from_flash)(struct dramc_param *dparam);
	bool (*write_to_flash)(const struct dramc_param *dparam);
};

struct dramc_param *get_dramc_param_from_blob(void *blob);
int validate_dramc_param(const void *blob);
int is_valid_dramc_param(const void *blob);
int initialize_dramc_param(void *blob, u16 config);

#endif  /* SOC_MEDIATEK_MT8183_DRAMC_PARAM_H */
