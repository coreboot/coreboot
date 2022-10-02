/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/vtxprintf.h>
#include <stdarg.h>
#include <string.h>

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

	ctx.str_buf = buf;
	ctx.buf_limit = size ? size - 1 : 0;
	i = vtxprintf(str_tx_byte, fmt, args, &ctx);
	if (size)
		*ctx.str_buf = '\0';

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
