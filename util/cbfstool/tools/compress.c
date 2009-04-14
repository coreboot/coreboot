/*
 * cbfstool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#include <string.h>

extern void do_lzma_compress(char *in, int in_len, char *out, int *out_len);

void lzma_compress(char *in, int in_len, char *out, int *out_len)
{
	do_lzma_compress(in, in_len, out, out_len);
}

void none_compress(char *in, int in_len, char *out, int *out_len)
{
	memcpy(out, in, in_len);
	*out_len = in_len;
}
