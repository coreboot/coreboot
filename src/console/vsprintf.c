/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <string.h>
#include <console/vtxprintf.h>
#include <trace.h>

struct context
{
	char *str_buf;
};

static void str_tx_byte(unsigned char byte, void *data)
{
	struct context *ctx = data;
	*ctx->str_buf = byte;
	ctx->str_buf++;
}

static int vsprintf(char *buf, const char *fmt, va_list args)
{
	int i;
	struct context ctx;

	DISABLE_TRACE;

	ctx.str_buf = buf;
	i = vtxdprintf(str_tx_byte, fmt, args, &ctx);
	*ctx.str_buf = '\0';

	ENABLE_TRACE;

	return i;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);

	return i;
}
