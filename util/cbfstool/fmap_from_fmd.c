/*
 * fmap_from_fmd.c, tool to distill flashmap descriptors into raw FMAP sections
 *
 * Copyright (C) 2015 Google, Inc.
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

#include "fmap_from_fmd.h"

#include "common.h"

#include <assert.h>
#include <string.h>

static bool fmap_append_fmd_node(struct fmap **flashmap,
				const struct flashmap_descriptor *section,
						unsigned absolute_watermark) {
	if (strlen(section->name) >= FMAP_STRLEN) {
		ERROR("Section name ('%s') exceeds %d character FMAP format limit\n",
						section->name, FMAP_STRLEN - 1);
		return false;
	}

	absolute_watermark += section->offset;

	if (fmap_append_area(flashmap, absolute_watermark, section->size,
					(uint8_t *)section->name, 0) < 0) {
		ERROR("Failed to insert section '%s' into FMAP\n",
								section->name);
		return false;
	}

	fmd_foreach_child(subsection, section) {
		if (!fmap_append_fmd_node(flashmap, subsection,
							absolute_watermark))
			return false;
	}

	return true;
}

struct fmap *fmap_from_fmd(const struct flashmap_descriptor *desc)
{
	assert(desc);
	assert(desc->size_known);

	if (strlen(desc->name) >= FMAP_STRLEN) {
		ERROR("Image name ('%s') exceeds %d character FMAP header limit\n",
						desc->name, FMAP_STRLEN - 1);
		return NULL;
	}

	struct fmap *fmap = fmap_create(desc->offset_known ? desc->offset : 0,
					desc->size, (uint8_t *)desc->name);
	if (!fmap) {
		ERROR("Failed to allocate FMAP header\n");
		return fmap;
	}

	fmd_foreach_child(real_section, desc) {
		if (!fmap_append_fmd_node(&fmap, real_section, 0)) {
			fmap_destroy(fmap);
			return NULL;
		}
	}

	return fmap;
}
