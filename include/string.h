/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef STRING_H
#define STRING_H

#include <types.h>
#include <shared.h>

/* lib/string.c */
size_t strnlen(const char *str, size_t maxlen);
size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int maxlen);

/* lib/mem.c */
SHARED(memcpy, void *, void *dest, const void *src, size_t len);
SHARED(memmove, void *, void *dest, const void *src, size_t len);
SHARED(memset, void *, void *s, int c, size_t len);
SHARED(memcmp, int , const void *s1, const void *s2, size_t len);

/* console/vsprintf.c */
int sprintf(char *buf, const char *fmt, ...);

#endif				/* STRING_H */
