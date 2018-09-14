/*
 * This file is part of the coreinfo project.
 *
 * Copyright (C) 2018 Google Inc.
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

#include <libpayload-config.h>
#include <libpayload.h>
#include "linuxcheck.h"

void buts(char *s)
{
	int i;
	for (i = 0; i < strlen(s); i++)
		outb(s[i], 0x3f8);
}

void hex4(u8 c)
{
	static char *hex = "0123456789abcdef";
	outb(hex[c & 0xf], 0x3f8);
}

void hex8(u8 c)
{
	hex4(c >> 4);
	hex4(c);
}

void hex16(u16 c)
{
	hex8((u8)(c >> 8));
	hex8((u8)c);
}
void hex32(u32 c)
{
	hex16((u16)(c >> 16));
	hex16((u16)c);
}
