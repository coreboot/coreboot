/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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


#include <arch/byteorder.h>
#include <boot/coreboot_tables.h>
#include <cbmem.h>
#include <cbfs.h>
#include <console/console.h>
#include <stdlib.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define CACHELINE_SIZE 64
#define INTRA_CACHELINE_MASK (CACHELINE_SIZE - 1)
#define CACHELINE_MASK (~INTRA_CACHELINE_MASK)

static void *find_mirror_buffer(int len)
{
	int nentries;
	int i;
	struct lb_memory *mem;
	void *buffer;

	len = ALIGN(len, 4096);

	mem = get_lb_mem();
	nentries = (mem->size - sizeof(*mem)) / sizeof(mem->map[0]);

	/*
	 * Find the highest RAM entry that accommodates the lenth provide
	 * while falling below 4GiB.
	 */
	buffer = NULL;
	for (i = 0; i < nentries; i++) {
		const uint64_t max_addr = 1ULL << 32;
		uint64_t start;
		uint64_t size;
		struct lb_memory_range *r;

		r = &mem->map[i];

		if (r->type != LB_MEM_RAM)
			continue;

		start = unpack_lb64(r->start);
		if (start >= max_addr)
			continue;

		size = unpack_lb64(r->size);
		if (size < len)
			continue;

		/* Adjust size of buffer if range exceeds max address. */
		if (start + size > max_addr)
			size = max_addr - start;

		if (size < len)
			continue;

		buffer = (void *)(uintptr_t)(start + size - len);
	}

	return buffer;
}

/*
 * Mirror the payload file to the default SMM location if it is small enough.
 * The default SMM region can be used since no one is using the memory at this
 * location at this stage in the boot.
 */
static void *spi_mirror(void *file_start, int file_len)
{
	int alignment_diff;
	char *src;
	char *dest;

	alignment_diff = (INTRA_CACHELINE_MASK & (long)file_start);

	/*
	 * Adjust file length so that the start and end points are aligned to a
	 * cacheline. Coupled with the ROM caching in the CPU the SPI hardware
	 * will read and cache full length cachelines. It will also prefetch
	 * data as well. Once things are mirrored in memory all accesses should
	 * hit the CPUs cache.
	 */
	file_len += alignment_diff;
	file_len = ALIGN(file_len, CACHELINE_SIZE);

	printk(BIOS_DEBUG, "Payload aligned size: 0x%x\n", file_len);

	dest = find_mirror_buffer(file_len);

	/*
	 * Just pass back the pointer to ROM space if a buffer could not
	 * be found to mirror into.
	 */
	if (dest == NULL)
		return file_start;

	src = (void *)(CACHELINE_MASK & (long)file_start);
	/*
	 * Note that if mempcy is not using 32-bit moves the performance will
	 * degrade because the SPI hardware prefetchers look for
	 * cacheline-aligned 32-bit accesses to kick in.
	 */
	memcpy(dest, src, file_len);

	/* Provide pointer into mirrored space. */
	return &dest[alignment_diff];
}

void *cbfs_load_payload(struct cbfs_media *media, const char *name)
{
	int file_len;
	void *file_start;
	struct cbfs_file *file;

	file_start = vboot_get_payload(&file_len);

	if (file_start != NULL)
		return spi_mirror(file_start, file_len);

	file = cbfs_get_file(media, name);

	if (file == NULL)
		return NULL;

	if (ntohl(file->type) != CBFS_TYPE_PAYLOAD)
		return NULL;

	file_len = ntohl(file->len);

	file_start = CBFS_SUBHEADER(file);

	return spi_mirror(file_start, file_len);
}
