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

			if (rdev_readat(&fh->metadata, &ftype,
					offsetof(struct cbfs_file, type),
					sizeof(ftype)) != sizeof(ftype))
				break;

			ftype = read_be32(&ftype);

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
