/*****************************************************************************\
 * opts.c
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

#include "common.h"
#include "opts.h"

nvramtool_op_info_t nvramtool_op;

nvramtool_op_modifier_info_t nvramtool_op_modifiers[NVRAMTOOL_NUM_OP_MODIFIERS];

static char *handle_optional_arg(int argc, char *argv[]);
static void register_op(int *op_found, nvramtool_op_t op, char op_param[]);
static void register_op_modifier(nvramtool_op_modifier_t mod, char mod_param[]);
static void resolve_op_modifiers(void);
static void sanity_check_args(void);

static const char getopt_string[] = "-ab:B:c::C:dD:e:hH:iL:l::np:r:tvw:xX:y:Y";

/****************************************************************************
 * parse_nvramtool_args
 *
 * Parse command line arguments.
 ****************************************************************************/
void parse_nvramtool_args(int argc, char *argv[])
{
	nvramtool_op_modifier_info_t *mod_info;
	int i, op_found;
	char c;

	for (i = 0, mod_info = nvramtool_op_modifiers;
	     i < NVRAMTOOL_NUM_OP_MODIFIERS; i++, mod_info++) {
		mod_info->found = FALSE;
		mod_info->found_seq = 0;
		mod_info->param = NULL;
	}

	op_found = FALSE;
	opterr = 0;

	do {
		switch (c = getopt(argc, argv, getopt_string)) {
		case 'a':
			register_op(&op_found,
				    NVRAMTOOL_OP_CMOS_SHOW_ALL_PARAMS, NULL);
			break;
		case 'b':
			register_op(&op_found, NVRAMTOOL_OP_WRITE_CMOS_DUMP,
				    optarg);
			break;
		case 'B':
			register_op(&op_found, NVRAMTOOL_OP_READ_CMOS_DUMP,
				    optarg);
			break;
		case 'c':
			register_op(&op_found, NVRAMTOOL_OP_CMOS_CHECKSUM,
				    handle_optional_arg(argc, argv));
			break;
		case 'C':
			register_op_modifier(NVRAMTOOL_MOD_USE_CBFS_FILE,
					     optarg);
			break;
		case 'd':
			register_op(&op_found, NVRAMTOOL_OP_LBTABLE_DUMP, NULL);
			break;
		case 'D':
			register_op_modifier(NVRAMTOOL_MOD_USE_CMOS_FILE,
					     optarg);
			break;
		case 'e':
			register_op(&op_found, NVRAMTOOL_OP_SHOW_PARAM_VALUES,
				    optarg);
			break;
		case 'h':
			register_op(&op_found, NVRAMTOOL_OP_SHOW_USAGE, NULL);
			break;
		case 'H':
			register_op(&op_found, NVRAMTOOL_OP_WRITE_HEADER_FILE, optarg);
			break;
		case 'i':
			register_op(&op_found,
				    NVRAMTOOL_OP_CMOS_SET_PARAMS_STDIN, NULL);
			break;
		case 'l':
			register_op(&op_found, NVRAMTOOL_OP_LBTABLE_SHOW_INFO,
				    handle_optional_arg(argc, argv));
			break;
		case 'L':
			register_op(&op_found, NVRAMTOOL_OP_WRITE_BINARY_FILE,
					     optarg);
			break;
		case 'n':
			register_op_modifier(NVRAMTOOL_MOD_SHOW_VALUE_ONLY,
					     NULL);
			break;
		case 'p':
			register_op(&op_found,
				    NVRAMTOOL_OP_CMOS_SET_PARAMS_FILE, optarg);
			break;
		case 'r':
			register_op(&op_found, NVRAMTOOL_OP_CMOS_SHOW_ONE_PARAM,
				    optarg);
			break;
		case 't':
			register_op_modifier(NVRAMTOOL_MOD_USE_CMOS_OPT_TABLE,
					     NULL);
			break;
		case 'v':
			register_op(&op_found, NVRAMTOOL_OP_SHOW_VERSION, NULL);
			break;
		case 'w':
			register_op(&op_found, NVRAMTOOL_OP_CMOS_SET_ONE_PARAM,
				    optarg);
			break;
		case 'x':
			register_op(&op_found, NVRAMTOOL_OP_SHOW_CMOS_HEX_DUMP,
				    NULL);
			break;
		case 'X':
			register_op(&op_found, NVRAMTOOL_OP_SHOW_CMOS_DUMPFILE,
				    optarg);
			break;
		case 'y':
			register_op_modifier(NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE,
					     optarg);
			break;
		case 'Y':
			register_op(&op_found, NVRAMTOOL_OP_SHOW_LAYOUT, NULL);
			break;
		case -1:	/* no more command line args */
			break;
		case '?':	/* unknown option found */
		case 1:	/* nonoption command line arg found */
		default:
			usage(stderr);
			break;
		}
	} while (c != -1);

	if (!op_found)
		usage(stderr);

	resolve_op_modifiers();
	sanity_check_args();
}

/****************************************************************************
 * handle_optional_arg
 *
 * Handle a command line option with an optional argument.
 ****************************************************************************/
static char *handle_optional_arg(int argc, char *argv[])
{
	char *arg;

	if (optarg != NULL) {
		/* optional arg is present and arg was specified as
		 * "-zarg" (with no whitespace between "z" and "arg"),
		 * where -z is the option and "arg" is the value of the
		 * optional arg
		 */
		return optarg;
	}

	if ((argv[optind] == NULL) || (argv[optind][0] == '-'))
		return NULL;

	arg = argv[optind];	/* optional arg is present */

	/* This call to getopt yields the optional arg we just found,
	 * which we want to skip.
	 */
	getopt(argc, argv, getopt_string);

	return arg;
}

/****************************************************************************
 * register_op
 *
 * Store the user's selection of which operation this program should perform.
 ****************************************************************************/
static void register_op(int *op_found, nvramtool_op_t op, char op_param[])
{
	if (*op_found && (op != nvramtool_op.op))
		usage(stderr);

	*op_found = TRUE;
	nvramtool_op.op = op;
	nvramtool_op.param = op_param;
}

/****************************************************************************
 * register_op_modifier
 *
 * Store information regarding an optional argument specified in addition to
 * the user's selection of which operation this program should perform.
 ****************************************************************************/
static void register_op_modifier(nvramtool_op_modifier_t mod, char mod_param[])
{
	static int found_seq = 0;
	nvramtool_op_modifier_info_t *mod_info;

	mod_info = &nvramtool_op_modifiers[mod];
	mod_info->found = TRUE;
	mod_info->found_seq = ++found_seq;
	mod_info->param = mod_param;
}

/****************************************************************************
 * resolve_op_modifiers
 *
 * If the user specifies multiple arguments that conflict with each other,
 * the last specified argument overrides previous conflicting arguments.
 ****************************************************************************/
static void resolve_op_modifiers(void)
{
	if (nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE].found &&
	    nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_OPT_TABLE].found) {
		if (nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE].found_seq >
		    nvramtool_op_modifiers[NVRAMTOOL_MOD_USE_CMOS_OPT_TABLE].found_seq)
			nvramtool_op_modifiers
			    [NVRAMTOOL_MOD_USE_CMOS_OPT_TABLE].found = FALSE;
		else
			nvramtool_op_modifiers
			    [NVRAMTOOL_MOD_USE_CMOS_LAYOUT_FILE].found = FALSE;
	}
}

/****************************************************************************
 * sanity_check_args
 *
 * Perform sanity checking on command line arguments.
 ****************************************************************************/
static void sanity_check_args(void)
{
	if ((nvramtool_op_modifiers[NVRAMTOOL_MOD_SHOW_VALUE_ONLY].found) &&
	    (nvramtool_op.op != NVRAMTOOL_OP_CMOS_SHOW_ONE_PARAM))
		usage(stderr);
}
