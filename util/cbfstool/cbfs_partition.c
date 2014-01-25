/*
 * Manipulators for CBFS partitions
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "cbfs_partition.h"
#include "endian_stream.h"
#include "../../payloads/external/GRUB2/grub2/grub-core/lib/libgcrypt-grub/src/types.h"

#include <string.h>

size_t cbfs_ptable_calc_size(size_t num_entries)
{
	/* The last 4 bytes are the table terminator */
	return CBFS_PARTITION_ENTRY_LEN * num_entries + 4;
}

void cbfs_partition_entry_init(struct cbfs_partition *part, const char *name,
			       uint32_t offset, uint32_t size, uint32_t fags)
{
	part->magic = CBFS_PARTITION_MAGIC;
	part->offset = offset;
	part->size = size;
	part->flags = fags;
	strncpy(part->name, name, CBFS_PARTITION_NAME_LEN);
}

void cbfs_partition_serialize(void *dest, const struct cbfs_partition *part)
{
	h_to_be32(part->magic, dest + 0);
	h_to_be32(part->offset, dest + 4);
	h_to_be32(part->size, dest + 8);
	h_to_be32(part->flags, dest + 12);
	memset(dest, 0xff, CBFS_PARTITION_NAME_LEN);
	strncpy(dest + 16, part->name, CBFS_PARTITION_NAME_LEN);
}

void cbfs_partition_deserialize(struct cbfs_partition *part, const void *src)
{
	part->magic = be32_to_h(src + 0);
	part->offset = be32_to_h(src + 4);
	part->size = be32_to_h(src + 8);
	part->flags = be32_to_h(src + 12);
	strncpy(part->name, src + 16, CBFS_PARTITION_NAME_LEN);
}

void cbfs_ptable_serialize(void *dest, const struct cbfs_partition *part)
{
	while (part->magic == CBFS_PARTITION_MAGIC) {
		cbfs_partition_serialize(dest, part);
		part++;
		dest += 16;
	}
}
