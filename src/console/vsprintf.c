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
#include <smp/spinlock.h>
#include <console/vtxprintf.h>
#include <trace.h>

DECLARE_SPIN_LOCK(vsprintf_lock)

static char *str_buf;
static size_t buf_limit;

static void str_tx_byte(unsigned char byte)
{
	if (buf_limit) {
		*str_buf = byte;
		str_buf++;
		buf_limit--;
	}
}

static int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	DISABLE_TRACE;
	spin_lock(&vsprintf_lock);

	str_buf = buf;
	buf_limit = size ? size - 1 : 0;
	i = vtxprintf(str_tx_byte, fmt, args);
	if (size)
		*str_buf = '\0';

	spin_unlock(&vsprintf_lock);
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
