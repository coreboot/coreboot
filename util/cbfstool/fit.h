/*
 * Firmware Interface Table support.
 *
 * Copyright (C) 2012 Google Inc.
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

#ifndef __CBFSTOOL_FIT_H
#define __CBFSTOOL_FIT_H

#include "cbfs_image.h"
#include "common.h"

/*
 * Converts between offsets from the start of the specified image region and
 * "top-aligned" offsets from the top of the entire flash image. Should work in
 * both directions: accepts either type of offset and produces the other type.
 * The implementation must have some notion of the flash image's total size.
 */
typedef unsigned (*fit_offset_converter_t)(const struct buffer *region,
							unsigned offset);

int fit_update_table(struct buffer *bootblock, struct cbfs_image *image,
		     const char *microcode_blob_name, int empty_entries,
		     fit_offset_converter_t offset_fn);
#endif
