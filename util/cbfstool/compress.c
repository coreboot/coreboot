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
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "lz4/lib/lz4frame.h"
#include <commonlib/compression.h>

static int lz4_compress(char *in, int in_len, char *out, int *out_len)
{
	LZ4F_preferences_t prefs = {
		.compressionLevel = 20,
		.frameInfo = {
			.blockSizeID = max4MB,
			.blockMode = blockIndependent,
			.contentChecksumFlag = noContentChecksum,
		},
	};
	size_t worst_size = LZ4F_compressFrameBound(in_len, &prefs);
	void *bounce = malloc(worst_size);
	if (!bounce)
		return -1;
	*out_len = LZ4F_compressFrame(bounce, worst_size, in, in_len, &prefs);
	if (LZ4F_isError(*out_len) || *out_len >= in_len)
		return -1;
	memcpy(out, bounce, *out_len);
	return 0;
}

static int lz4_decompress(char *in, int in_len, char *out, int out_len,
			  size_t *actual_size)
{
	size_t result = ulz4fn(in, in_len, out, out_len);
	if (result == 0)
		return -1;
	if (actual_size != NULL)
		*actual_size = result;
	return 0;
}

static int lzma_compress(char *in, int in_len, char *out, int *out_len)
{
	return do_lzma_compress(in, in_len, out, out_len);
}

static int lzma_decompress(char *in, int in_len, char *out, unused int out_len,
				size_t *actual_size)
{
	return do_lzma_uncompress(out, out_len, in, in_len, actual_size);
}
static int none_compress(char *in, int in_len, char *out, int *out_len)
{
	memcpy(out, in, in_len);
	*out_len = in_len;
	return 0;
}

static int none_decompress(char *in, int in_len, char *out, unused int out_len,
				size_t *actual_size)
{
	memcpy(out, in, in_len);
	if (actual_size != NULL)
		*actual_size = in_len;
	return 0;
}

comp_func_ptr compression_function(enum comp_algo algo)
{
	comp_func_ptr compress;
	switch (algo) {
	case CBFS_COMPRESS_NONE:
		compress = none_compress;
		break;
	case CBFS_COMPRESS_LZMA:
		compress = lzma_compress;
		break;
	case CBFS_COMPRESS_LZ4:
		compress = lz4_compress;
		break;
	default:
		ERROR("Unknown compression algorithm %d!\n", algo);
		return NULL;
	}
	return compress;
}

decomp_func_ptr decompression_function(enum comp_algo algo)
{
	decomp_func_ptr decompress;
	switch (algo) {
	case CBFS_COMPRESS_NONE:
		decompress = none_decompress;
		break;
	case CBFS_COMPRESS_LZMA:
		decompress = lzma_decompress;
		break;
	case CBFS_COMPRESS_LZ4:
		decompress = lz4_decompress;
		break;
	default:
		ERROR("Unknown compression algorithm %d!\n", algo);
		return NULL;
	}
	return decompress;
}
