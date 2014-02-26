/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <console/console.h>
#include <bootmem.h>
#include <payload_loader.h>

void mirror_payload(struct payload *payload)
{
	char *buffer;
	size_t size;
	char *src;
	uintptr_t alignment_diff;
	const unsigned long cacheline_size = 64;
	const uintptr_t intra_cacheline_mask = cacheline_size - 1;
	const uintptr_t cacheline_mask = ~intra_cacheline_mask;

	src = payload->backing_store.data;
	size = payload->backing_store.size;

	/*
	 * Adjust size so that the start and end points are aligned to a
	 * cacheline. The SPI hardware controllers on Intel machines should
	 * cache full length cachelines as well as prefetch data.  Once the
	 * data is mirrored in memory all accesses should hit the CPU's cache.
	 */
	alignment_diff = (intra_cacheline_mask & (uintptr_t)src);
	size += alignment_diff;

	size = ALIGN(size, cacheline_size);

	printk(BIOS_DEBUG, "Payload aligned size: 0x%zx\n", size);

	buffer = bootmem_allocate_buffer(size);

	if (buffer == NULL) {
		printk(BIOS_DEBUG, "No buffer for mirroring payload.\n");
		return;
	}

	src = (void *)(cacheline_mask & (uintptr_t)src);

	/*
	 * Note that if mempcy is not using 32-bit moves the performance will
	 * degrade because the SPI hardware prefetchers look for
	 * cacheline-aligned 32-bit accesses to kick in.
	 */
	memcpy(buffer, src, size);

	/* Update the payload's backing store. */
	payload->backing_store.data = &buffer[alignment_diff];
}
