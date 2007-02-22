/*
 * mem -- simple routines that have no optimizations for anything
 * 
 *
 * Copright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */

/* this one is pretty stupid. Won't handle overlaps, it's not efficient, etc. */
void memcpy(void *dest, const void *src, int len)
{
	unsigned char *d = dest;
	const unsigned char *s = src;
	while (len--)
		*d++ = *s++;
}

void memset(void *v, unsigned char a, int len)
{
	unsigned char *cp = v;
	while(len--)
		*cp++ = a;
}

