/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/bsd/helpers.h>
#include <device/mmio.h>
#include <types.h>

/* Helper functions for various MMIO access patterns. */

void buffer_from_fifo32(void *buffer, size_t size, void *fifo,
			int fifo_stride, int fifo_width)
{
	u8 *p = buffer;
	int i, j;

	assert(fifo_width > 0 && fifo_width <= sizeof(u32) &&
	       fifo_stride % sizeof(u32) == 0);

	for (i = 0; i < size; i += fifo_width, fifo += fifo_stride) {
		u32 val = read32(fifo);
		for (j = 0; j < MIN(size - i, fifo_width); j++)
			*p++ = (u8)(val >> (j * 8));
	}
}

void buffer_to_fifo32_prefix(const void *buffer, u32 prefix, int prefsz, size_t size,
			     void *fifo, int fifo_stride, int fifo_width)
{
	const u8 *p = buffer;
	int i, j = prefsz;

	assert(fifo_width > 0 && fifo_width <= sizeof(u32) &&
	       fifo_stride % sizeof(u32) == 0 && prefsz <= fifo_width);

	uint32_t val = prefix;
	for (i = 0; i < size; i += fifo_width, fifo += fifo_stride) {
		for (; j < MIN(size - i, fifo_width); j++)
			val |= *p++ << (j * 8);
		write32(fifo, val);
		val = 0;
		j = 0;
	}

}
