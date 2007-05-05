/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Peter Stuge <peter@stuge.se>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/* Memory routines with some optimizations. Please don't be silly and inline
 * these. Inlines are not as wonderful as people think.
 */

#include <arch/types.h>
#include <string.h>

/**
 * memcpy() and memmove() helper that uses unsigned long copying when dest and
 * src are both unsigned long aligned, or offset by the same amount from being
 * unsigned long aligned.
 *
 * Copies data one byte at a time until both dest and src are aligned to
 * sizeof(unsigned long) or until no data is left.
 *
 * Copies data one unsigned long at a time until fewer than
 * sizeof(unsigned long) bytes remain.
 *
 * Finally, or if dest and src have different alignment, copies any remaining
 * data one byte at a time.
 *
 * @param dest Memory area to copy to.
 * @param src Memory area to copy from.
 * @param len Number of bytes to copy.
 * @param backwards Start at the end, used by memmove() when dest > src.
 */
static void memcpy_helper(void *dest, const void *src, size_t len,
			  int backwards)
{
	u8 *d = dest;
	const u8 *s = src;
	unsigned long *ld;
	const unsigned long *ls;
	u8 longmask = sizeof(unsigned long) - 1;
	u8 longlen = sizeof(unsigned long);

	if (d == s || !len)
		return;

	if (backwards) {
		d += len;
		s += len;
		if (((unsigned long)d & longmask) ==
		    ((unsigned long)s & longmask)) {
			while (((unsigned long)d & longmask
				|| (unsigned long)s & longmask) && len) {
				*--d = *--s;
				len--;
			}
			ld = (unsigned long *)d;
			ls = (const unsigned long *)s;
			while (len >= longlen) {
				*--ld = *--ls;
				len -= longlen;
			}
			d = (u8 *) ld;
			s = (const u8 *)ls;
		}
		while (len--)
			*--d = *--s;
	} else {
		if (((unsigned long)d & longmask) ==
		    ((unsigned long)s & longmask)) {
			while (((unsigned long)d & longmask
				|| (unsigned long)s & longmask) && len) {
				*d++ = *s++;
				len--;
			}
			ld = (unsigned long *)d;
			ls = (const unsigned long *)s;
			while (len >= longlen) {
				*ld++ = *ls++;
				len -= longlen;
			}
			d = (u8 *) ld;
			s = (const u8 *)ls;
		}
		while (len--)
			*d++ = *s++;
	}
}

/**
 * Copy 'len' bytes from one memory area to another.
 *
 * The memory areas may _not_ overlap.
 *
 * @param dest Pointer to the destination memory area.
 * @param src Pointer to the source memory area.
 * @param len Number of bytes to copy.
 * @return Pointer specified by parameter dest
 */
void *memcpy(void *dest, const void *src, size_t len)
{
	memcpy_helper(dest, src, len, 0);
	return dest;
}

/**
 * Copy 'len' bytes from one memory area to another.
 *
 * The memory areas may overlap.
 *
 * @param dest Pointer to the destination memory area.
 * @param src Pointer to the source memory area.
 * @param len Number of bytes to copy.
 * @return Pointer specified by parameter dest
 */
void *memmove(void *dest, const void *src, size_t len)
{
	memcpy_helper(dest, src, len, dest > src && dest < (src + len));
	return dest;
}

/**
 * Fill a memory area with the specified byte.
 *
 * @param s Pointer to the beginning of the memory area.
 * @param c The byte which is used for filling the memory area.
 * @param len The number of bytes to write.
 * @return Pointer specified by parameter s 
 */
void *memset(void *s, int c, size_t len)
{
	unsigned char *cp = s;
	while (len--)
		*cp++ = (unsigned char)c;
	return s;
}

/**
 * Compare the first 'len' bytes of two memory areas.
 *
 * We assume unsigned characters here.
 *
 * @param s1 Pointer to the first memory area.
 * @param s2 Pointer to the second memory area.
 * @param len Number of bytes to compare.
 * @return Returns a negative number if s1 is shorter than s2. Returns zero if
 * 	   s1 matches s2. Returns a positive number if s1 is longer than s2.
 */
int memcmp(const void *s1, const void *s2, size_t len)
{
	const unsigned char *d = (const unsigned char *)s1;
	const unsigned char *s = (const unsigned char *)s2;
	while (len--) {
		if (*d < *s)
			return -1;
		if (*d > *s)
			return 1;
		d++, s++;
	}
	return 0;
}
