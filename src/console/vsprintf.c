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
 */

#include <console/vtxprintf.h>
#include <string.h>
#include <trace.h>

struct vsnprintf_context {
	char *str_buf;
	size_t buf_limit;
};

static void str_tx_byte(unsigned char byte, void *data)
{
	struct vsnprintf_context *ctx = data;
	if (ctx->buf_limit) {
		*ctx->str_buf = byte;
		ctx->str_buf++;
		ctx->buf_limit--;
	}
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;
	struct vsnprintf_context ctx;

	DISABLE_TRACE;

	ctx.str_buf = buf;
	ctx.buf_limit = size ? size - 1 : 0;
	i = vtxprintf(str_tx_byte, fmt, args, &ctx);
	if (size)
		*ctx.str_buf = '\0';

	ENABLE_TRACE;

	return i;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}
