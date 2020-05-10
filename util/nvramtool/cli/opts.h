/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef OPTS_H
#define OPTS_H

#include "common.h"

typedef enum { NVRAMTOOL_OP_SHOW_VERSION = 0,
	NVRAMTOOL_OP_SHOW_USAGE,
	NVRAMTOOL_OP_LBTABLE_SHOW_INFO,
	NVRAMTOOL_OP_LBTABLE_DUMP,
	NVRAMTOOL_OP_SHOW_PARAM_VALUES,
	NVRAMTOOL_OP_CMOS_SHOW_ONE_PARAM,
	NVRAMTOOL_OP_CMOS_SHOW_ALL_PARAMS,
	NVRAMTOOL_OP_CMOS_SET_ONE_PARAM,
	NVRAMTOOL_OP_CMOS_SET_PARAMS_STDIN,
	NVRAMTOOL_OP_CMOS_SET_PARAMS_FILE,
	NVRAMTOOL_OP_CMOS_CHECKSUM,
	NVRAMTOOL_OP_SHOW_LAYOUT,
	NVRAMTOOL_OP_WRITE_CMOS_DUMP,
	NVRAMTOOL_OP_READ_CMOS_DUMP,
	NVRAMTOOL_OP_SHOW_CMOS_HEX_DUMP,
	NVRAMTOOL_OP_SHOW_CMOS_DUMPFILE,
	NVRAMTOOL_OP_WRITE_BINARY_FILE,
	NVRAMTOOL_OP_WRITE_HEADER_FILE
} nvramtool_op_t;

typedef struct {
	nvramtool_op_t op;
	char *param;
} nvramtool_op_info_t;

typedef enum { NVRAMTOOL_MOD_SHOW_VALUE_ONLY = 0,
	NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE,
	NVRAMTOOL_MOD_USE_CBFS_FILE,
	NVRAMTOOL_MOD_USE_CMOS_FILE,
	NVRAMTOOL_MOD_USE_CMOS_OPT_TABLE,
	NVRAMTOOL_NUM_OP_MODIFIERS	/* must always be last */
} nvramtool_op_modifier_t;

typedef struct {
	int found;
	int found_seq;
	char *param;
} nvramtool_op_modifier_info_t;

extern nvramtool_op_info_t nvramtool_op;

extern nvramtool_op_modifier_info_t nvramtool_op_modifiers[];

void parse_nvramtool_args(int argc, char *argv[]);

#endif				/* OPTS_H */
