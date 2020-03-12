/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONSOLE_VTXPRINTF_H
#define __CONSOLE_VTXPRINTF_H

#include <stdarg.h>

int vtxprintf(void (*tx_byte)(unsigned char byte, void *data),
	const char *fmt, va_list args, void *data);

#endif
