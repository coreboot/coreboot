/* compression handling for cbfstool */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "lz4/lib/lz4frame.h"
#include <zstd.h>
#include <commonlib/bsd/compression.h>

static int
lz4_compress(char *in, int in_len, char *out, int *out_len)
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
	if (LZ4F_isError(*out_len) || *out_len >= in_len) {
		free(bounce);
		return -1;
	}
	memcpy(out, bounce, *out_len);
	free(bounce);
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

static int zstd_compress(char *in, int in_len, char *out, int *out_len)
{
	size_t worst_size = ZSTD_compressBound(in_len);
	size_t ret;

	void *bounce = malloc(worst_size);
	if (!bounce)
		return -1;
	ret = ZSTD_compress(bounce, worst_size, in, in_len, ZSTD_maxCLevel());
	if (ZSTD_isError(ret)) {
		ERROR("ZSTD_compress (v%s) returned %zu (%s)\n",
		      ZSTD_versionString(), ret, ZSTD_getErrorName(ret));
		free(bounce);
		return -2;
	}
	if (ret >= (size_t)in_len) {
		free(bounce);
		return -3;
	}
	*out_len = (int)ret;

	memcpy(out, bounce, *out_len);
	free(bounce);
	return 0;
}

static int zstd_decompress(char *in, int in_len, char *out, int out_len,
			   size_t *actual_size)
{
	ZSTD_DCtx* dctx = ZSTD_createDCtx();
	size_t ret = ZSTD_decompressDCtx(dctx, out, out_len, in, in_len);
	ZSTD_freeDCtx(dctx);

	if (ret == 0)
		return -1;
	if (ZSTD_isError(ret)) {
		ERROR("ZSTD_decompress (v%s) returned %zu (%s)\n",
		      ZSTD_versionString(), ret, ZSTD_getErrorName(ret));
		return -2;
	}
	if (actual_size != NULL)
		*actual_size = ret;
	return 0;
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

comp_func_ptr compression_function(enum cbfs_compression algo)
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
	case CBFS_COMPRESS_ZSTD:
		compress = zstd_compress;
		break;
	default:
		ERROR("Unknown compression algorithm %d!\n", algo);
		return NULL;
	}
	return compress;
}

decomp_func_ptr decompression_function(enum cbfs_compression algo)
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
	case CBFS_COMPRESS_ZSTD:
		decompress = zstd_decompress;
		break;
	default:
		ERROR("Unknown compression algorithm %d!\n", algo);
		return NULL;
	}
	return decompress;
}
