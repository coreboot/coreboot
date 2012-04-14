/*****************************************************************************\
 * opts.h
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of nvramtool, a utility for reading/writing coreboot
 *  parameters and displaying information from the coreboot table.
 *  For details, see http://coreboot.org/nvramtool.
 *
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
\*****************************************************************************/

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
