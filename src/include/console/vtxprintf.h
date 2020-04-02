/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __CONSOLE_VTXPRINTF_H
#define __CONSOLE_VTXPRINTF_H

#include <stdarg.h>

int vtxprintf(void (*tx_byte)(unsigned char byte, void *data),
	const char *fmt, va_list args, void *data);

#endif
