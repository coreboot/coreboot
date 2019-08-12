/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __DEVICE_MMIO_H__
#define __DEVICE_MMIO_H__

#include <arch/mmio.h>
#include <endian.h>
#include <types.h>

#ifndef __ROMCC__
/*
 * Reads a transfer buffer from 32-bit FIFO registers. fifo_stride is the
 * distance in bytes between registers (e.g. pass 4 for a normal array of 32-bit
 * registers or 0 to read everything from the same register). fifo_width is
 * the amount of bytes read per register (can be 1 through 4).
 */
void buffer_from_fifo32(void *buffer, size_t size, void *fifo,
			int fifo_stride, int fifo_width);

/*
 * Version of buffer_to_fifo32() that can prepend a prefix of up to fifo_width
 * size to the transfer. This is often useful for protocols where a command word
 * precedes the actual payload data. The prefix must be packed in the low-order
 * bytes of the 'prefix' u32 parameter and any high-order bytes exceeding prefsz
 * must be 0. Note that 'size' counts total bytes written, including 'prefsz'.
 */
void buffer_to_fifo32_prefix(void *buffer, u32 prefix, int prefsz, size_t size,
			     void *fifo, int fifo_stride, int fifo_width);

/*
 * Writes a transfer buffer into 32-bit FIFO registers. fifo_stride is the
 * distance in bytes between registers (e.g. pass 4 for a normal array of 32-bit
 * registers or 0 to write everything into the same register). fifo_width is
 * the amount of bytes written per register (can be 1 through 4).
 */
static inline void buffer_to_fifo32(void *buffer, size_t size, void *fifo,
				    int fifo_stride, int fifo_width)
{
	buffer_to_fifo32_prefix(buffer, size, 0, 0, fifo,
				fifo_stride, fifo_width);
}
#endif	/* !__ROMCC__ */

#endif	/* __DEVICE_MMIO_H__ */
