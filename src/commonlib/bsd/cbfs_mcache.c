/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/bsd/cbfs_private.h>

/*
 * A CBFS metadata cache is an in memory data structure storing CBFS file headers (= metadata).
 * It is defined by its start pointer and size. It contains a sequence of variable-length
 * union mcache_entry entries. There is no overall header structure for the cache.
 *
 * Each mcache_entry is the raw metadata for a CBFS file (including attributes) in the same form
 * as stored on flash (i.e. values in big-endian), except that the CBFS magic signature in the
 * first 8 bytes ('LARCHIVE') is overwritten with mcache-internal bookkeeping data. The first 4
 * bytes are a magic number (MCACHE_MAGIC_FILE) and the next 4 bytes are the absolute offset in
 * bytes on the cbfs_dev_t that this metadata blob was found at. (Note that depending on the
 * implementation of cbfs_dev_t, this offset may still be relative to the start of a subregion
 * of the underlying storage device.)
 *
 * The length of an mcache_entry (i.e. length of the underlying metadata blob) is encoded in the
 * metadata (entry->file.h.offset). The next mcache_entry begins at the next
 * CBFS_MCACHE_ALIGNMENT boundary after that. The cache is terminated by a special 4-byte
 * mcache_entry that consists only of a magic number (MCACHE_MAGIC_END or MCACHE_MAGIC_FULL).
 */

#define MCACHE_MAGIC_FILE	0x454c4946	/* 'FILE' */
#define MCACHE_MAGIC_FULL	0x4c4c5546	/* 'FULL' */
#define MCACHE_MAGIC_END	0x444e4524	/* '$END' */

union mcache_entry {
	union cbfs_mdata file;
	struct {	/* These fields exactly overlap file.h.magic */
		uint32_t magic;
		uint32_t offset;
	};
};

struct cbfs_mcache_build_args {
	void *mcache;
	void *end;
	int count;
};

static enum cb_err build_walker(cbfs_dev_t dev, size_t offset, const union cbfs_mdata *mdata,
				size_t already_read, void *arg)
{
	struct cbfs_mcache_build_args *args = arg;
	union mcache_entry *entry = args->mcache;
	const uint32_t data_offset = be32toh(mdata->h.offset);

	if (args->end - args->mcache < data_offset)
		return CB_CBFS_CACHE_FULL;

	if (cbfs_copy_fill_metadata(args->mcache, mdata, already_read, dev, offset))
		return CB_CBFS_IO;

	entry->magic = MCACHE_MAGIC_FILE;
	entry->offset = offset;

	args->mcache += ALIGN_UP(data_offset, CBFS_MCACHE_ALIGNMENT);
	args->count++;

	return CB_CBFS_NOT_FOUND;
}

enum cb_err cbfs_mcache_build(cbfs_dev_t dev, void *mcache, size_t size,
			      struct vb2_hash *metadata_hash)
{
	struct cbfs_mcache_build_args args = {
		.mcache = mcache,
		.end = mcache + ALIGN_DOWN(size, CBFS_MCACHE_ALIGNMENT)
		       - sizeof(uint32_t), /* leave space for terminating magic */
		.count = 0,
	};

	assert(size > sizeof(uint32_t) && IS_ALIGNED((uintptr_t)mcache, CBFS_MCACHE_ALIGNMENT));
	enum cb_err ret = cbfs_walk(dev, build_walker, &args, metadata_hash, 0);
	union mcache_entry *entry = args.mcache;
	if (ret == CB_CBFS_NOT_FOUND) {
		ret = CB_SUCCESS;
		entry->magic = MCACHE_MAGIC_END;
	} else if (ret == CB_CBFS_CACHE_FULL) {
		ERROR("mcache overflow, should increase CBFS_MCACHE size!\n");
		entry->magic = MCACHE_MAGIC_FULL;
	}

	LOG("mcache @%p built for %d files, used %#zx of %#zx bytes\n", mcache,
	    args.count, args.mcache + sizeof(entry->magic) - mcache, size);
	return ret;
}

enum cb_err cbfs_mcache_lookup(const void *mcache, size_t mcache_size, const char *name,
			       union cbfs_mdata *mdata_out, size_t *data_offset_out)
{
	const size_t namesize = strlen(name) + 1; /* Count trailing \0 so we can memcmp() it. */
	const void *end = mcache + mcache_size;
	const void *current = mcache;

	while (current + sizeof(uint32_t) <= end) {
		const union mcache_entry *entry = current;

		if (entry->magic == MCACHE_MAGIC_END)
			return CB_CBFS_NOT_FOUND;
		if (entry->magic == MCACHE_MAGIC_FULL)
			return CB_CBFS_CACHE_FULL;

		assert(entry->magic == MCACHE_MAGIC_FILE);
		const uint32_t data_offset = be32toh(entry->file.h.offset);
		const uint32_t data_length = be32toh(entry->file.h.len);
		if (namesize <= data_offset - offsetof(union cbfs_mdata, h.filename) &&
		    memcmp(name, entry->file.h.filename, namesize) == 0) {
			LOG("Found '%s' @%#x size %#x in mcache @%p\n",
			    name, entry->offset, data_length, current);
			*data_offset_out = entry->offset + data_offset;
			memcpy(mdata_out, &entry->file, data_offset);
			return CB_SUCCESS;
		}

		current += ALIGN_UP(data_offset, CBFS_MCACHE_ALIGNMENT);
	}

	ERROR("CBFS mcache is not terminated!\n");	/* should never happen */
	return CB_ERR;
}

size_t cbfs_mcache_real_size(const void *mcache, size_t mcache_size)
{
	const void *end = mcache + mcache_size;
	const void *current = mcache;

	while (current + sizeof(uint32_t) <= end) {
		const union mcache_entry *entry = current;

		if (entry->magic == MCACHE_MAGIC_FULL || entry->magic == MCACHE_MAGIC_END) {
			current += sizeof(entry->magic);
			break;
		}

		assert(entry->magic == MCACHE_MAGIC_FILE);
		current += ALIGN_UP(be32toh(entry->file.h.offset), CBFS_MCACHE_ALIGNMENT);
	}

	return current - mcache;
}
