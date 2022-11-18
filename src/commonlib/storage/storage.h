/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __COMMONLIB_STORAGE_STORAGE_H__
#define __COMMONLIB_STORAGE_STORAGE_H__

#include <stdint.h>
#include <commonlib/storage.h>

#define DMA_MINALIGN (64)
#define ROUND(a, b) (((a) + (b) - 1) & ~((b) - 1))
#define ALLOC_CACHE_ALIGN_BUFFER(type, name, size)                   \
	char __##name[ROUND(size * sizeof(type), DMA_MINALIGN) +     \
		      DMA_MINALIGN - 1];                             \
	type *name = (type *)ALIGN_UP((uintptr_t)__##name, DMA_MINALIGN)

/* NOOPs mirroring ARM's cache API, since x86 devices usually cache snoop */
#define dcache_invalidate_by_mva(addr, len)
#define dcache_clean_invalidate_by_mva(addr, len)

/* Storage support routines */
int storage_startup(struct storage_media *media);
int storage_block_setup(struct storage_media *media, uint64_t start,
	uint64_t count, int is_read);

#endif /* __COMMONLIB_STORAGE_STORAGE_H__ */
