/*
 * compression handling for cbfstool
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 *
 * Adapted from code
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>, released
 * under identical license terms
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
#include <stdio.h>
#include "common.h"

void do_lzma_compress(char *in, int in_len, char *out, int *out_len);

static void lzma_compress(char *in, int in_len, char *out, int *out_len)
{
	do_lzma_compress(in, in_len, out, out_len);
}

static void none_compress(char *in, int in_len, char *out, int *out_len)
{
	memcpy(out, in, in_len);
	*out_len = in_len;
}

comp_func_ptr compression_function(comp_algo algo)
{
	comp_func_ptr compress;
	switch (algo) {
	case CBFS_COMPRESS_NONE:
		compress = none_compress;
		break;
	case CBFS_COMPRESS_LZMA:
		compress = lzma_compress;
		break;
	default:
		ERROR("Unknown compression algorithm %d!\n", algo);
		return NULL;
	}
	return compress;
}
