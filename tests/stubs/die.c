/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdarg.h>
#include <tests/test.h>

void die(const char *msg, ...)
{
	/* die() can be called in middle a function, so we should not allow for it to return */
	static char msg_buf[256];
	va_list v;
	va_start(v, msg);
	vsnprintf(msg_buf, ARRAY_SIZE(msg_buf), msg, v);
	va_end(v);
	fail_msg("%s", msg_buf);
}
