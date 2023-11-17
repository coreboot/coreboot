/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "storage.h"

#include <assert.h>
#include <stdio.h>

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

	bool auth_vars;
	if (!fv_parse(storage->file, &storage->store_area, &auth_vars)) {
		if (!rw) {
			fprintf(stderr,
				"Failed to find variable store in \"%s\"\n",
				store_file);
			goto error;
		}

		if (!fv_init(storage->file)) {
			fprintf(stderr,
				"Failed to create variable store in \"%s\"\n",
				store_file);
			goto error;
		}

		if (!fv_parse(storage->file, &storage->store_area, &auth_vars)) {
			fprintf(stderr,
				"Failed to parse newly formatted store in \"%s\"\n",
				store_file);
			goto error;
		}

		fprintf(stderr,
			"Successfully created variable store in \"%s\"\n",
			store_file);
	}

	storage->vs = vs_load(storage->store_area, auth_vars);
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
