/* SPDX-License-Identifier: GPL-2.0-only */

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
