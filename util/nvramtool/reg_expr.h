/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef REG_EXPR_H
#define REG_EXPR_H

#include <regex.h>
#include "common.h"

void compile_reg_expr(int cflags, const char *expr, regex_t *reg);

#endif				/* REG_EXPR_H */
