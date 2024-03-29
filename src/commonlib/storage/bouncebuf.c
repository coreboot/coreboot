/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Generic bounce buffer implementation
 */

#include <arch/cache.h>
#include "bouncebuf.h"
#include "storage.h"
#include <string.h>
#include <commonlib/bsd/stdlib.h>

static int addr_aligned(struct bounce_buffer *state)
{
	const uint32_t align_mask = ARCH_DMA_MINALIGN - 1;

	// Check if start is aligned
	if ((uintptr_t)state->user_buffer & align_mask) {
		sdhc_debug("Unaligned buffer address %p\n", state->user_buffer);
		return 0;
	}

	// Check if length is aligned
	if (state->len != state->len_aligned) {
		sdhc_debug("Unaligned buffer length %zd\n", state->len);
		return 0;
	}

	// Aligned
	return 1;
}

int bounce_buffer_start(struct bounce_buffer *state, void *data,
			size_t len, unsigned int flags)
{
	state->user_buffer = data;
	state->bounce_buffer = data;
	state->len = len;
	state->len_aligned = ROUND(len, ARCH_DMA_MINALIGN);
	state->flags = flags;

	if (!addr_aligned(state)) {
		state->bounce_buffer = memalign(ARCH_DMA_MINALIGN,
						state->len_aligned);
		if (!state->bounce_buffer)
			return -1;

		if (state->flags & GEN_BB_READ)
			memcpy(state->bounce_buffer, state->user_buffer,
				state->len);
	}

	/*
	 * Flush data to RAM so DMA reads can pick it up,
	 * and any CPU writebacks don't race with DMA writes
	 */
	dcache_clean_invalidate_by_mva(state->bounce_buffer,
				       state->len_aligned);
	return 0;
}

int bounce_buffer_stop(struct bounce_buffer *state)
{
	if (state->flags & GEN_BB_WRITE) {
		// Invalidate cache so that CPU can see any newly DMA'd data
		dcache_invalidate_by_mva(state->bounce_buffer,
					 state->len_aligned);
	}

	if (state->bounce_buffer == state->user_buffer)
		return 0;

	if (state->flags & GEN_BB_WRITE)
		memcpy(state->user_buffer, state->bounce_buffer, state->len);

	free(state->bounce_buffer);

	return 0;
}
