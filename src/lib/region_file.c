/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <commonlib/helpers.h>
#include <console/console.h>
#include <region_file.h>
#include <string.h>

/*
 * A region file provides generic support for appending new data
 * within a storage region. The book keeping is tracked in metadata
 * blocks where an offset pointer points to the last byte of a newly
 * allocated byte sequence. Thus, by taking 2 block offets one can
 * determine start and size of the latest update. The data does not
 * have to be the same consistent size, but the data size has be small
 * enough to fit a metadata block and one data write within the region.
 *
 * The granularity of the block offsets are 16 bytes. By using 16-bit
 * block offsets a region's total size can be no larger than 1MiB.
 * However, the last 32 bytes cannot be used in the 1MiB maximum region
 * because one needs to put a block offset indicating last byte written.
 * An unused block offset is the value 0xffff or 0xffff0 bytes. The last
 * block offset that can be written is 0xfffe or 0xfffe0 byte offset.
 *
 * The goal of this library is to provide a simple mechanism for
 * allocating blocks of data for updates. The metadata is written first
 * followed by the data. That means a power event between the block offset
 * write and the data write results in blocks being allocated but not
 * entirely written. It's up to the user of the library to sanity check
 * data stored.
 */

#define REGF_BLOCK_SHIFT		4
#define REGF_BLOCK_GRANULARITY		(1 << REGF_BLOCK_SHIFT)
#define REGF_METADATA_BLOCK_SIZE	REGF_BLOCK_GRANULARITY
#define REGF_UNALLOCATED_BLOCK		0xffff
#define REGF_UPDATES_PER_METADATA_BLOCK	\
	(REGF_METADATA_BLOCK_SIZE / sizeof(uint16_t))

enum {
	RF_ONLY_METADATA = 0,
	RF_EMPTY = -1,
	RF_NEED_TO_EMPTY = -2,
	RF_FATAL = -3,
};

struct metadata_block {
	uint16_t blocks[REGF_UPDATES_PER_METADATA_BLOCK];
};

static size_t block_to_bytes(uint16_t offset)
{
	return (size_t)offset << REGF_BLOCK_SHIFT;
}

static size_t bytes_to_block(size_t bytes)
{
	return bytes >> REGF_BLOCK_SHIFT;
}

static inline int block_offset_unallocated(uint16_t offset)
{
	return offset == REGF_UNALLOCATED_BLOCK;
}

static inline size_t region_file_data_begin(const struct region_file *f)
{
	return f->data_blocks[0];
}

static inline size_t region_file_data_end(const struct region_file *f)
{
	return f->data_blocks[1];
}

static int all_block_offsets_unallocated(const struct metadata_block *mb)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(mb->blocks); i++) {
		if (!block_offset_unallocated(mb->blocks[i]))
			return 0;
	}

	return 1;
}

/* Read metadata block at block i. */
static int read_mb(size_t i, struct metadata_block *mb,
			const struct region_file *f)
{
	size_t offset = block_to_bytes(i);

	if (rdev_readat(&f->metadata, mb, offset, sizeof(*mb)) < 0)
		return -1;

	return 0;
}

/* Locate metadata block with the latest update */
static int find_latest_mb(struct metadata_block *mb, size_t num_mb_blocks,
				struct region_file *f)
{
	size_t l = 0;
	size_t r = num_mb_blocks;

	while (l + 1 < r) {
		size_t mid = (l + r) / 2;

		if (read_mb(mid, mb, f) < 0)
			return -1;
		if (all_block_offsets_unallocated(mb))
			r = mid;
		else
			l = mid;
	}

	/* Set the base block slot. */
	f->slot = l * REGF_UPDATES_PER_METADATA_BLOCK;

	/* Re-read metadata block with the latest update. */
	if (read_mb(l, mb, f) < 0)
		return -1;

	return 0;
}

static void find_latest_slot(struct metadata_block *mb, struct region_file *f)
{
	size_t i;

	for (i = REGF_UPDATES_PER_METADATA_BLOCK - 1; i > 0; i--) {
		if (!block_offset_unallocated(mb->blocks[i]))
			break;
	}

	f->slot += i;
}

static int fill_data_boundaries(struct region_file *f)
{
	struct region_device slots;
	size_t offset;
	size_t size = sizeof(f->data_blocks);

	if (f->slot == RF_ONLY_METADATA) {
		size_t start = bytes_to_block(region_device_sz(&f->metadata));
		f->data_blocks[0] = start;
		f->data_blocks[1] = start;
		return 0;
	}

	/* Sanity check the 2 slot sequence to read. If it's out of the
	 * metadata blocks' bounds then one needs to empty it. This is done
	 * to uniquely identify I/O vs data errors in the readat() below. */
	offset = (f->slot - 1) * sizeof(f->data_blocks[0]);
	if (rdev_chain(&slots, &f->metadata, offset, size)) {
		f->slot = RF_NEED_TO_EMPTY;
		return 0;
	}

	if (rdev_readat(&slots, &f->data_blocks, 0, size) < 0) {
		printk(BIOS_ERR, "REGF failed to read data boundaries.\n");
		return -1;
	}

	/* All used blocks should be incrementing from previous write. */
	if (region_file_data_begin(f) >= region_file_data_end(f)) {
		printk(BIOS_ERR, "REGF data boundaries wrong. [%zd,%zd) Need to empty.\n",
			region_file_data_begin(f), region_file_data_end(f));
		f->slot = RF_NEED_TO_EMPTY;
		return 0;
	}

	/* Ensure data doesn't exceed the region. */
	if (region_file_data_end(f) >
		bytes_to_block(region_device_sz(&f->rdev))) {
		printk(BIOS_ERR, "REGF data exceeds region %zd > %zd\n",
			region_file_data_end(f),
			bytes_to_block(region_device_sz(&f->rdev)));
		f->slot = RF_NEED_TO_EMPTY;
	}

	return 0;
}

int region_file_init(struct region_file *f, const struct region_device *p)
{
	struct metadata_block mb;

	/* Total number of metadata blocks is found by reading the first
	 * block offset as the metadata is allocated first. At least one
	 * metadata block is available. */

	memset(f, 0, sizeof(*f));
	f->slot = RF_FATAL;

	/* Keep parent around for accessing data later. */
	if (rdev_chain(&f->rdev, p, 0, region_device_sz(p)))
		return -1;

	if (rdev_readat(p, &mb, 0, sizeof(mb)) < 0) {
		printk(BIOS_ERR, "REGF fail reading first metadata block.\n");
		return -1;
	}

	/* No metadata has been allocated. Assume region is empty. */
	if (block_offset_unallocated(mb.blocks[0])) {
		f->slot = RF_EMPTY;
		return 0;
	}

	/* If metadata block is 0 in size then need to empty. */
	if (mb.blocks[0] == 0) {
		f->slot = RF_NEED_TO_EMPTY;
		return 0;
	}

	/* The region needs to be emptied as the metadata is broken. */
	if (rdev_chain(&f->metadata, p, 0, block_to_bytes(mb.blocks[0]))) {
		f->slot = RF_NEED_TO_EMPTY;
		return 0;
	}

	/* Locate latest metadata block with latest update. */
	if (find_latest_mb(&mb, mb.blocks[0], f)) {
		printk(BIOS_ERR, "REGF fail locating latest metadata block.\n");
		f->slot = RF_FATAL;
		return -1;
	}

	find_latest_slot(&mb, f);

	/* Fill in the data blocks marking the latest update. */
	if (fill_data_boundaries(f)) {
		printk(BIOS_ERR, "REGF fail locating data boundaries.\n");
		f->slot = RF_FATAL;
		return -1;
	}

	return 0;
}

int region_file_data(const struct region_file *f, struct region_device *rdev)
{

	size_t offset;
	size_t size;

	/* Slot indicates if any data is available. */
	if (f->slot <= RF_ONLY_METADATA)
		return -1;

	offset = block_to_bytes(region_file_data_begin(f));
	size = block_to_bytes(region_file_data_end(f)) - offset;

	return rdev_chain(rdev, &f->rdev, offset, size);
}

/*
 * Allocate enough metadata blocks to maximize data updates. Do this in
 * terms of blocks. To solve the balance of metadata vs data, 2 linear
 * equations are solved in terms of blocks where 'x' is number of
 * data updates and 'y' is number of metadata blocks:
 *
 *   x = number of data updates
 *   y = number of metadata blocks
 *   T = total blocks in region
 *   D = data size in blocks
 *   M = metadata size in blocks
 *   A = updates accounted for in each metadata block
 *
 *   T = D * x + M * y
 *   y = x / A
 *   -----------------
 *   T = D * x + M * x / A = x * (D + M / A)
 *   T * A = x * (D * A + M)
 *   x = T * A / (D * A + M)
 */
static int allocate_metadata(struct region_file *f, size_t data_blks)
{
	size_t t, m;
	size_t x, y;
	uint16_t tot_metadata;
	const size_t a = REGF_UPDATES_PER_METADATA_BLOCK;
	const size_t d = data_blks;

	t = bytes_to_block(ALIGN_DOWN(region_device_sz(&f->rdev),
					REGF_BLOCK_GRANULARITY));
	m = bytes_to_block(ALIGN_UP(REGF_METADATA_BLOCK_SIZE,
					REGF_BLOCK_GRANULARITY));

	/* Ensure at least one data update can fit with 1 metadata block
	 * within the region. */
	if (d > t - m)
		return -1;

	/* Maximize number of updates by aligning up to the number updates in
	 * a metadata block. May not really be able to achieve the number of
	 * updates in practice, but it ensures enough metadata blocks are
	 * allocated. */
	x = ALIGN_UP(t * a / (d * a + m), a);

	/* One data block has to fit. */
	if (x == 0)
		x = 1;

	/* Now calculate how many metadata blocks are needed. */
	y = ALIGN_UP(x, a) / a;

	/* Need to commit the metadata allocation. */
	tot_metadata = m * y;
	if (rdev_writeat(&f->rdev, &tot_metadata, 0, sizeof(tot_metadata)) < 0)
		return -1;

	if (rdev_chain(&f->metadata, &f->rdev, 0,
				block_to_bytes(tot_metadata)))
		return -1;

	/* Initialize a 0 data block to start appending from. */
	f->data_blocks[0] = tot_metadata;
	f->data_blocks[1] = tot_metadata;

	return 0;
}

static int update_can_fit(const struct region_file *f, size_t data_blks)
{
	size_t metadata_slots;
	size_t end_blk;

	metadata_slots = region_device_sz(&f->metadata) / sizeof(uint16_t);

	/* No more slots. */
	if ((size_t)f->slot + 1 >= metadata_slots)
		return 0;

	/* See where the last block lies from the current one. */
	end_blk = data_blks + region_file_data_end(f);

	/* Update would have exceeded block addressing. */
	if (end_blk >= REGF_UNALLOCATED_BLOCK)
		return 0;

	/* End block exceeds size of region. */
	if (end_blk > bytes_to_block(region_device_sz(&f->rdev)))
		return 0;

	return 1;
}

static int commit_data_allocation(struct region_file *f, size_t data_blks)
{
	size_t offset;

	f->slot++;

	offset = f->slot * sizeof(uint16_t);
	f->data_blocks[0] = region_file_data_end(f);
	f->data_blocks[1] = region_file_data_begin(f) + data_blks;

	if (rdev_writeat(&f->metadata, &f->data_blocks[1], offset,
				sizeof(f->data_blocks[1])) < 0)
		return -1;

	return 0;
}

static int commit_data(const struct region_file *f, const void *buf,
			size_t size)
{
	size_t offset = block_to_bytes(region_file_data_begin(f));
	if (rdev_writeat(&f->rdev, buf, offset, size) < 0)
		return -1;
	return 0;
}

static int handle_empty(struct region_file *f, size_t data_blks)
{
	if (allocate_metadata(f, data_blks)) {
		printk(BIOS_ERR, "REGF metadata allocation failed: %zd data blocks %zd total blocks\n",
			data_blks, bytes_to_block(region_device_sz(&f->rdev)));
		return -1;
	}

	f->slot = RF_ONLY_METADATA;

	return 0;
}

static int handle_need_to_empty(struct region_file *f)
{
	if (rdev_eraseat(&f->rdev, 0, region_device_sz(&f->rdev)) < 0) {
		printk(BIOS_ERR, "REGF empty failed.\n");
		return -1;
	}

	f->slot = RF_EMPTY;

	return 0;
}

static int handle_update(struct region_file *f, size_t blocks, const void *buf,
				size_t size)
{
	if (!update_can_fit(f, blocks)) {
		printk(BIOS_INFO, "REGF update can't fit. Will empty.\n");
		f->slot = RF_NEED_TO_EMPTY;
		return 0;
	}

	if (commit_data_allocation(f, blocks)) {
		printk(BIOS_ERR, "REGF failed to commit data allocation.\n");
		return -1;
	}

	if (commit_data(f, buf, size)) {
		printk(BIOS_ERR, "REGF failed to commit data.\n");
		return -1;
	}

	return 0;
}

int region_file_update_data(struct region_file *f, const void *buf, size_t size)
{
	int ret;
	size_t blocks;

	blocks = bytes_to_block(ALIGN_UP(size, REGF_BLOCK_GRANULARITY));

	while (1) {
		int prev_slot = f->slot;

		switch (f->slot) {
		case RF_EMPTY:
			ret = handle_empty(f, blocks);
			break;
		case RF_NEED_TO_EMPTY:
			ret = handle_need_to_empty(f);
			break;
		case RF_FATAL:
			ret = -1;
			break;
		default:
			ret = handle_update(f, blocks, buf, size);
			break;
		}

		/* Failing case. No more updates allowed to be attempted. */
		if (ret) {
			f->slot = RF_FATAL;
			break;
		}

		/* No more state changes and data committed. */
		if (f->slot > RF_ONLY_METADATA && prev_slot != f->slot)
			break;
	}

	return ret;
}
