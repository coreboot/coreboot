/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <console/console.h>
#include <commonlib/cbfs.h>
#include <commonlib/endian.h>
#include <commonlib/helpers.h>
#include <string.h>

#if !defined(ERROR)
#define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
#endif
#if !defined(LOG)
#define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
#endif
#if defined(IS_ENABLED)

#if IS_ENABLED(CONFIG_DEBUG_CBFS)
#define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
#else
#define DEBUG(x...)
#endif

#elif !defined(DEBUG)
#define DEBUG(x...)
#endif

static size_t cbfs_next_offset(const struct region_device *cbfs,
				const struct cbfsf *f)
{
	size_t offset;

	if (f == NULL)
		return 0;

	/* The region_device objects store absolute offets over the whole
	 * region. Therefore a relative offset needs to be calculated. */
	offset = rdev_relative_offset(cbfs, &f->data);
	offset += region_device_sz(&f->data);

	return ALIGN_UP(offset, CBFS_ALIGNMENT);
}

static int cbfs_end(const struct region_device *cbfs, size_t offset)
{
	if (offset >= region_device_sz(cbfs))
		return 1;

	return 0;
}

int cbfs_for_each_file(const struct region_device *cbfs,
			const struct cbfsf *prev, struct cbfsf *fh)
{
	size_t offset;

	offset = cbfs_next_offset(cbfs, prev);

	/* Try to scan the entire cbfs region looking for file name. */
	while (1) {
		struct cbfs_file file;
		const size_t fsz = sizeof(file);

		 DEBUG("Checking offset %zx\n", offset);

		/* End of region. */
		if (cbfs_end(cbfs, offset))
			return 1;

		/* Can't read file. Nothing else to do but bail out. */
		if (rdev_readat(cbfs, &file, offset, fsz) != fsz)
			break;

		if (memcmp(file.magic, CBFS_FILE_MAGIC, sizeof(file.magic))) {
			offset++;
			offset = ALIGN_UP(offset, CBFS_ALIGNMENT);
			continue;
		}

		file.len = read_be32(&file.len);
		file.offset = read_be32(&file.offset);

		DEBUG("File @ offset %zx size %x\n", offset, file.len);

		/* Keep track of both the metadata and the data for the file. */
		if (rdev_chain(&fh->metadata, cbfs, offset, file.offset))
			break;

		if (rdev_chain(&fh->data, cbfs, offset + file.offset, file.len))
			break;

		/* Success. */
		return 0;
	}

	return -1;
}

size_t cbfs_for_each_attr(void *metadata, size_t metadata_size,
			  size_t last_offset)
{
	struct cbfs_file_attribute *attr;

	if (!last_offset) {
		struct cbfs_file *file = metadata;
		size_t start_offset = read_be32(&file->attributes_offset);
		if (start_offset <= sizeof(struct cbfs_file) ||
		    start_offset + sizeof(*attr) > metadata_size)
			return 0;
		return start_offset;
	}

	attr = metadata + last_offset;
	size_t next_offset = last_offset + read_be32(&attr->len);

	if (next_offset + sizeof(*attr) > metadata_size)
		return 0;
	return next_offset;
}

int cbfsf_decompression_info(struct cbfsf *fh, uint32_t *algo, size_t *size)
{
	size_t metadata_size = region_device_sz(&fh->metadata);
	void *metadata = rdev_mmap_full(&fh->metadata);
	size_t offs = 0;

	if (!metadata)
		return -1;

	while ((offs = cbfs_for_each_attr(metadata, metadata_size, offs))) {
		struct cbfs_file_attr_compression *attr = metadata + offs;
		if (read_be32(&attr->tag) != CBFS_FILE_ATTR_TAG_COMPRESSION)
			continue;

		*algo = read_be32(&attr->compression);
		*size = read_be32(&attr->decompressed_size);
		rdev_munmap(&fh->metadata, metadata);
		return 0;
	}

	*algo = CBFS_COMPRESS_NONE;
	*size = region_device_sz(&fh->data);
	rdev_munmap(&fh->metadata, metadata);
	return 0;
}

static int cbfsf_file_type(struct cbfsf *fh, uint32_t *ftype)
{
	const size_t sz = sizeof(*ftype);

	if (rdev_readat(&fh->metadata, ftype,
			offsetof(struct cbfs_file, type), sz) != sz)
		return -1;

	*ftype = read_be32(ftype);

	return 0;
}

int cbfs_locate(struct cbfsf *fh, const struct region_device *cbfs,
		const char *name, uint32_t *type)
{
	struct cbfsf *prev;

	LOG("Locating '%s'\n", name);

	prev = NULL;

	while (1) {
		int ret;
		char *fname;
		int name_match;
		const size_t fsz = sizeof(struct cbfs_file);

		ret = cbfs_for_each_file(cbfs, prev, fh);
		prev = fh;

		/* Either failed to read or hit the end of the region. */
		if (ret < 0 || ret > 0)
			break;

		fname = rdev_mmap(&fh->metadata, fsz,
				region_device_sz(&fh->metadata) - fsz);

		if (fname == NULL)
			break;

		name_match = !strcmp(fname, name);
		rdev_munmap(&fh->metadata, fname);

		if (!name_match) {
			DEBUG(" Unmatched '%s' at %zx\n", fname,
				rdev_relative_offset(cbfs, &fh->metadata));
			continue;
		}

		if (type != NULL) {
			uint32_t ftype;

			if (cbfsf_file_type(fh, &ftype))
				break;

			if (*type != ftype) {
				DEBUG(" Unmatched type %x at %zx\n", ftype,
					rdev_relative_offset(cbfs,
							&fh->metadata));
				continue;
			}
		}

		LOG("Found @ offset %zx size %zx\n",
			rdev_relative_offset(cbfs, &fh->metadata),
			region_device_sz(&fh->data));

		/* Success. */
		return 0;
	}

	LOG("'%s' not found.\n", name);
	return -1;
}

static int cbfs_extend_hash_buffer(struct vb2_digest_context *ctx,
					void *buf, size_t sz)
{
	return vb2_digest_extend(ctx, buf, sz);
}

static int cbfs_extend_hash(struct vb2_digest_context *ctx,
				const struct region_device *rdev)
{
	uint8_t buffer[1024];
	size_t sz_left;
	size_t offset;

	sz_left = region_device_sz(rdev);
	offset = 0;

	while (sz_left) {
		int rv;
		size_t block_sz = MIN(sz_left, sizeof(buffer));

		if (rdev_readat(rdev, buffer, offset, block_sz) != block_sz)
			return VB2_ERROR_UNKNOWN;

		rv = cbfs_extend_hash_buffer(ctx, buffer, block_sz);

		if (rv)
			return rv;

		sz_left -= block_sz;
		offset += block_sz;
	}

	return VB2_SUCCESS;
}

/* Include offsets of child regions within the parent into the hash. */
static int cbfs_extend_hash_with_offset(struct vb2_digest_context *ctx,
					const struct region_device *p,
					const struct region_device *c)
{
	int32_t soffset;
	int rv;

	soffset = rdev_relative_offset(p, c);

	if (soffset < 0)
		return VB2_ERROR_UNKNOWN;

	/* All offsets in big endian format. */
	write_be32(&soffset, soffset);

	rv = cbfs_extend_hash_buffer(ctx, &soffset, sizeof(soffset));

	if (rv)
		return rv;

	return cbfs_extend_hash(ctx, c);
}

/* Hash in the potential CBFS header sitting at the beginning of the CBFS
 * region as well as relative offset at the end. */
static int cbfs_extend_hash_master_header(struct vb2_digest_context *ctx,
					const struct region_device *cbfs)
{
	struct region_device rdev;
	int rv;

	if (rdev_chain(&rdev, cbfs, 0, sizeof(struct cbfs_header)))
		return VB2_ERROR_UNKNOWN;

	rv = cbfs_extend_hash_with_offset(ctx, cbfs, &rdev);

	if (rv)
		return rv;

	/* Include potential relative offset at end of region. */
	if (rdev_chain(&rdev, cbfs, region_device_sz(cbfs) - sizeof(int32_t),
			sizeof(int32_t)))
		return VB2_ERROR_UNKNOWN;

	return cbfs_extend_hash_with_offset(ctx, cbfs, &rdev);
}

int cbfs_vb2_hash_contents(const struct region_device *cbfs,
				enum vb2_hash_algorithm hash_alg, void *digest,
				size_t digest_sz)
{
	struct vb2_digest_context ctx;
	int rv;
	struct cbfsf f;
	struct cbfsf *prev;
	struct cbfsf *fh;

	rv = vb2_digest_init(&ctx, hash_alg);

	if (rv)
		return rv;

	rv = cbfs_extend_hash_master_header(&ctx, cbfs);
	if (rv)
		return rv;

	prev = NULL;
	fh = &f;

	while (1) {
		uint32_t ftype;

		rv = cbfs_for_each_file(cbfs, prev, fh);
		prev = fh;

		if (rv < 0)
			return VB2_ERROR_UNKNOWN;

		/* End of CBFS. */
		if (rv > 0)
			break;

		rv = cbfs_extend_hash_with_offset(&ctx, cbfs, &fh->metadata);

		if (rv)
			return rv;

		/* Include data contents in hash if file is non-empty. */
		if (cbfsf_file_type(fh, &ftype))
			return VB2_ERROR_UNKNOWN;

		if (ftype == CBFS_TYPE_DELETED || ftype == CBFS_TYPE_DELETED2)
			continue;

		rv = cbfs_extend_hash_with_offset(&ctx, cbfs, &fh->data);

		if (rv)
			return rv;
	}

	return vb2_digest_finalize(&ctx, digest, digest_sz);
}
