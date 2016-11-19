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

#ifndef REGION_FILE_H
#define REGION_FILE_H

#include <commonlib/region.h>
#include <stdint.h>

/*
 * A region file is an abstraction to allow appending updates in a
 * region_device where the data returned is the most recently written
 * data. It is block based with a 16 byte granularity. So if you write
 * 2 bytes into the file the data returned in the region_device would
 * have 16 bytes allocated for the latest update. Additionally, the
 * current maximum file size allowed is 1MiB - 48 bytes. See comments
 * in C implementation file for further details.
 */

struct region_file;

/*
 * Initialize a region file associated with a provided region device.
 * Returns < 0 on error, 0 on success.
 */
int region_file_init(struct region_file *f, const struct region_device *p);

/*
 * Initialize region device object associated with latest update of file data.
 * Returns < 0 on error, 0 on success.
 */
int region_file_data(const struct region_file *f, struct region_device *rdev);

/* Update region file with latest data. Returns < 0 on error, 0 on success. */
int region_file_update_data(struct region_file *f, const void *buf,
				size_t size);

/* Declared here for easy object allocation. */
struct region_file {
	/* Region device covering file */
	struct region_device rdev;
	/* Metadata containing blocks of the data stream. */
	struct region_device metadata;
	/* Blocks forming data. */
	uint16_t data_blocks[2];
	/* Current slot in metadata marking end of data. */
	int slot;
};

#endif /* REGION_FILE_H */
