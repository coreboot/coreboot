/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#include <assert.h>
#include <commonlib/bsd/compression.h>
#include <stdint.h>
#include <stddef.h>
#define ZSTD_STATIC_LINKING_ONLY 1
#include <zstd.h>
#undef ZSTD_STATIC_LINKING_ONLY
#include <zstd_decompress_internal.h>

size_t uzstdn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	static ZSTD_DCtx dctx __aligned(8);
	if (!ZSTD_initStaticDCtx(&dctx, sizeof(dctx)))
		return 0;
	return ZSTD_decompressDCtx(&dctx, dst, dstn, src, srcn);
}
