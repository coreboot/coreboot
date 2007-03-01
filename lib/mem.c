/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Peter Stuge <peter@stuge.se>
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

/* Memory routines with some optimizations. */

#include <arch/types.h>

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
static void memcpy_helper(void *dest, const void *src, int len, int backwards) {
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
		if (((unsigned long)d & longmask) == ((unsigned long)s & longmask)) {
			while (((unsigned long)d & longmask || (unsigned long)s & longmask) && len) {
				*--d = *--s;
				len--;
			}
			ld = (unsigned long *)d;
			ls = (const unsigned long *)s;
			while (len >= longlen) {
				*--ld = *--ls;
				len -= longlen;
			}
			d = (u8 *)ld;
			s = (const u8 *)ls;
		}
		while (len--)
			*--d = *--s;
	} else {
		if (((unsigned long)d & longmask) == ((unsigned long)s & longmask)) {
			while (((unsigned long)d & longmask || (unsigned long)s & longmask) && len) {
				*d++ = *s++;
				len--;
			}
			ld = (unsigned long *)d;
			ls = (const unsigned long *)s;
			while (len >= longlen) {
				*ld++ = *ls++;
				len -= longlen;
			}
			d = (u8 *)ld;
			s = (const u8 *)ls;
		}
		while (len--)
			*d++ = *s++;
	}
}

/* Won't handle overlaps. */
/* Please don't be silly and inline these. Inlines are not as wonderful as people think */
void memcpy(void *dest, const void *src, int len)
{
	memcpy_helper(dest, src, len, 0);
}

/* Handles overlapping memory. */
/* seperate function in case we decide to use the built-in -- not sure yet. */
void memmove(void *dest, const void *src, int len)
{
	memcpy_helper(dest, src, len, dest > src && dest < (src + len));
}

void memset(void *v, unsigned char a, int len)
{
	unsigned char *cp = v;
	while(len--)
		*cp++ = a;
}

/* did you ever notice that the memcmp web page does not specify 
  * a signed or unsigned compare? It matters ... oh well, we assumed unsigned
  */
int memcmp(const void *s1, const void *s2, int len)
{
	const unsigned char *d = (const unsigned char *)s1;
	const unsigned char *s = (const unsigned char *)s2;
	while (len--){
		if (*d < *s)
			return -1;
		if (*d > *s)
			return 1;
		d++, s++;
	}
	return 0;

}

