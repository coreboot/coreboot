/*****************************************************************************\
 * reg_expr.h
 *****************************************************************************
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
\*****************************************************************************/

#ifndef REG_EXPR_H
#define REG_EXPR_H

#include <regex.h>
#include "common.h"

void compile_reg_expr(int cflags, const char *expr, regex_t *reg);

#endif				/* REG_EXPR_H */
