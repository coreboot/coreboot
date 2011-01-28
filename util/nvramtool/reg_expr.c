/*****************************************************************************\
 * reg_expr.c
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

#include <stdarg.h>
#include "common.h"
#include "reg_expr.h"

/****************************************************************************
 * compile_reg_expr
 *
 * Compile a regular expression.
 ****************************************************************************/
void compile_reg_expr(int cflags, const char *expr, regex_t *reg)
{
	static const size_t ERROR_BUF_SIZE = 256;
	char error_msg[ERROR_BUF_SIZE];
	int result;

	if ((result = regcomp(reg, expr, cflags)) != 0) {
		regerror(result, reg, error_msg, ERROR_BUF_SIZE);
		fprintf(stderr, "%s: %s\n", prog_name, error_msg);
		exit(1);
	}
}

