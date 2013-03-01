/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
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


#include <stdlib.h>
#include <string.h>
#include <arch/byteorder.h>
#include <cbmem.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#if CONFIG_VBOOT_VERIFY_FIRMWARE
#include <vendorcode/google/chromeos/chromeos.h>
#else
static inline void *vboot_get_payload(int *len) { return NULL; }
#endif

#define CACHELINE_SIZE 64
#define INTRA_CACHELINE_MASK (CACHELINE_SIZE - 1)
#define CACHELINE_MASK (~INTRA_CACHELINE_MASK)

/* Mirror the payload file to the default SMM location if it is small enough.
 * The default SMM region can be used since no one is using the memory at this
 * location at this stage in the boot. */
static inline void *spi_mirror(void *file_start, int file_len)
{
	int alignment_diff;
	char *src;
	char *dest = (void *)SMM_DEFAULT_BASE;

	alignment_diff = (INTRA_CACHELINE_MASK & (long)file_start);

	/* Adjust file length so that the start and end points are aligned to a
	 * cacheline. Coupled with the ROM caching in the CPU the SPI hardware
	 * will read and cache full length cachelines. It will also prefetch
	 * data as well. Once things are mirrored in memory all accesses should
	 * hit the CPUs cache. */
	file_len += alignment_diff;
	file_len = ALIGN(file_len, CACHELINE_SIZE);

	printk(BIOS_DEBUG, "Payload aligned size: 0x%x\n", file_len);

	/* Just pass back the pointer to ROM space if the file is larger
	 * than the RAM mirror region. */
	if (file_len > SMM_DEFAULT_SIZE)
		return file_start;

	src = (void *)(CACHELINE_MASK & (long)file_start);
	/* Note that if mempcy is not using 32-bit moves the performance will
	 * degrade because the SPI hardware prefetchers look for
	 * cacheline-aligned 32-bit accesses to kick in. */
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
