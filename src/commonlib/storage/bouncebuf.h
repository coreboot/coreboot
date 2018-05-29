/*
 * Generic bounce buffer implementation
 *
 * Copyright (C) 2012 Marek Vasut <marex@denx.de>
 * Copyright 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __COMMONLIB_STORAGE_BOUNCEBUF_H__
#define __COMMONLIB_STORAGE_BOUNCEBUF_H__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * GEN_BB_READ -- Data are read from the buffer eg. by DMA hardware.
 * The source buffer is copied into the bounce buffer (if unaligned, otherwise
 * the source buffer is used directly) upon start() call, then the operation
 * requiring the aligned transfer happens, then the bounce buffer is lost upon
 * stop() call.
 */
#define GEN_BB_READ	(1 << 0)
/*
 * GEN_BB_WRITE -- Data are written into the buffer eg. by DMA hardware.
 * The source buffer starts in an undefined state upon start() call, then the
 * operation requiring the aligned transfer happens, then the bounce buffer is
 * copied into the destination buffer (if unaligned, otherwise destination
 * buffer is used directly) upon stop() call.
 */
#define GEN_BB_WRITE	(1 << 1)
/*
 * GEN_BB_RW -- Data are read and written into the buffer eg. by DMA hardware.
 * The source buffer is copied into the bounce buffer (if unaligned, otherwise
 * the source buffer is used directly) upon start() call, then the  operation
 * requiring the aligned transfer happens, then the bounce buffer is  copied
 * into the destination buffer (if unaligned, otherwise destination buffer is
 * used directly) upon stop() call.
 */
#define GEN_BB_RW	(GEN_BB_READ | GEN_BB_WRITE)

struct bounce_buffer {
	/* Copy of data parameter passed to start() */
	void *user_buffer;
	/*
	 * DMA-aligned buffer. This field is always set to the value that
	 * should be used for DMA; either equal to .user_buffer, or to a
	 * freshly allocated aligned buffer.
	 */
	void *bounce_buffer;
	/* Copy of len parameter passed to start() */
	size_t len;
	/* DMA-aligned buffer length */
	size_t len_aligned;
	/* Copy of flags parameter passed to start() */
	unsigned int flags;
};

/**
 * bounce_buffer_start() -- Start the bounce buffer session
 * state:	stores state passed between bounce_buffer_{start,stop}
 * data:	pointer to buffer to be aligned
 * len:		length of the buffer
 * flags:	flags describing the transaction, see above.
 */
int bounce_buffer_start(struct bounce_buffer *state, void *data,
			size_t len, unsigned int flags);
/**
 * bounce_buffer_stop() -- Finish the bounce buffer session
 * state:	stores state passed between bounce_buffer_{start,stop}
 */
int bounce_buffer_stop(struct bounce_buffer *state);

// TODO(hungte) Eliminate the alignment stuff below and replace them with a
// better and centralized way to handler non-cache/aligned memory.
// Helper macros for alignment.
#define DMA_MINALIGN (64)
#define ROUND(a, b) (((a) + (b) - 1) & ~((b) - 1))
#define ALLOC_CACHE_ALIGN_BUFFER(type, name, size)                   \
	char __##name[ROUND(size * sizeof(type), DMA_MINALIGN) +     \
		      DMA_MINALIGN - 1];                             \
	type *name = (type *) ALIGN((uintptr_t)__##name, DMA_MINALIGN)
#ifndef ARCH_DMA_MINALIGN
#define ARCH_DMA_MINALIGN (DMA_MINALIGN)
#endif

#endif // __COMMONLIB_STORAGE_BOUNCEBUF_H__
