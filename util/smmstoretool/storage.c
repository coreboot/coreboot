/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "storage.h"

#include <assert.h>
#include <stdio.h>

#include "commonlib/bsd/compiler.h"
#include "fmap.h"

#include "fv.h"
#include "utils.h"

bool storage_open(const char store_file[], struct storage_t *storage, bool rw)
{
	storage->rw = rw;

	storage->file = map_file(store_file, rw);
	if (storage->file.start == NULL) {
		fprintf(stderr, "Failed to load smm-store-file \"%s\"\n",
			store_file);
		return false;
	}

	/* If we won't find FMAP with SMMSTORE, use the whole file, but fail if
	 * FMAP is there without SMMSTORE. */
	storage->region = storage->file;

	long fmap_offset = fmap_find(storage->file.start, storage->file.length);
	if (fmap_offset >= 0) {
		struct fmap *fmap = (void *)(storage->file.start + fmap_offset);
		const struct fmap_area *area = fmap_find_area(fmap, "SMMSTORE");
		if (area == NULL) {
			fprintf(stderr,
				"Found FMAP without SMMSTORE in \"%s\"\n",
				store_file);
			return false;
		}

		storage->region.start += area->offset;
		storage->region.length = area->size;
	}

	bool is_auth_var_store;
	if (!fv_parse(storage->region, &storage->store_area, &is_auth_var_store)) {
		if (!rw) {
			fprintf(stderr,
				"Failed to find variable store in \"%s\"\n",
				store_file);
			goto error;
		}

		fprintf(stderr,
			"\nThe variable store has not been found in the ROM image\n"
			"and is about to be initialized. This situation is normal\n"
			"for a release image, as the variable store is usually\n"
			"initialized on the first boot of the platform.\n\n");

		if (!fv_init(storage->region)) {
			fprintf(stderr,
				"Failed to create variable store in \"%s\"\n",
				store_file);
			goto error;
		}

		if (!fv_parse(storage->region, &storage->store_area, &is_auth_var_store)) {
			fprintf(stderr,
				"Failed to parse newly formatted store in \"%s\"\n",
				store_file);
			goto error;
		}

		fprintf(stderr,
			"Successfully created variable store in \"%s\"\n",
			store_file);
	}

	storage->vs = vs_load(storage->store_area, is_auth_var_store);
	return true;

error:
	unmap_file(storage->file);
	return false;
}

bool storage_write_back(struct storage_t *storage)
{
	assert(storage->rw && "Only RW storage can be updated.");

	bool success = vs_store(&storage->vs, storage->store_area);
	if (!success)
		fprintf(stderr, "Failed to update variable store\n");
	storage_drop(storage);
	return success;
}

void storage_drop(struct storage_t *storage)
{
	unmap_file(storage->file);
	vs_free(&storage->vs);
}
