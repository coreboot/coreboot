/*
 * Prototypes for manipulating for CBFS partitions
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef __CBFS_PARTITION_H
#define __CBFS_PARTITION_H

#include "cbfs.h"
#include <stddef.h>

void cbfs_partition_entry_init(struct cbfs_partition *part, const char *name,
			       uint32_t offset, uint32_t size, uint32_t flags);
void cbfs_partition_serialize(void *dest, const struct cbfs_partition *part);
void cbfs_partition_deserialize(struct cbfs_partition *part, const void *src);
size_t cbfs_ptable_calc_size(size_t num_entries);
void cbfs_ptable_serialize(void *dest, const struct cbfs_partition *part);
#endif	/* __CBFS_PARTITION_H */
