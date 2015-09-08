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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
#ifndef ASSETS_H
#define ASSETS_H

#include <commonlib/region.h>

/* An asset represents data used to boot the system. It can be found within
 * CBFS or some other mechanism. While CBFS can be a source of an asset, note
 * that using the asset API implies querying of other sources. */

enum asset_type {
	ASSET_UNKNOWN,
	ASSET_VERSTAGE,
	ASSET_ROMSTAGE,
	ASSET_RAMSTAGE,
	ASSET_REFCODE,
	ASSET_PAYLOAD,
	ASSET_BL31,
	ASSET_BL32,
};

struct asset {
	enum asset_type type;
	const char *name;
	struct region_device rdev;
};

static inline const char *asset_name(const struct asset *asset)
{
	return asset->name;
}

static inline enum asset_type asset_type(const struct asset *asset)
{
	return asset->type;
}

static struct region_device *asset_rdev(struct asset *asset)
{
	return &asset->rdev;
}

static inline size_t asset_size(const struct asset *asset)
{
	return region_device_sz(&asset->rdev);
}

/* Returns NULL on error. */
static inline void *asset_mmap(const struct asset *asset)
{
	return rdev_mmap_full(&asset->rdev);
}

#define ASSET_INIT(type_, name_)	\
	{				\
		.type = (type_),	\
		.name = (name_),	\
	}

/* Locate the asset as described by the parameter. It will query all known
 * asset providers. Returns 0 on success. < 0 on error. */
int asset_locate(struct asset *asset);

struct asset_provider {
	const char *name;
	/* Determines if the provider is the active one. If so returns 1 else 0
	 * or < 0 on error. */
	int (*is_active)(struct asset *asset);
	/* Returns < 0 on error or 0 on success. This function locates
	 * the rdev representing the file data associated with the passed in
	 * prog. */
	int (*locate)(struct asset *asset);
};

#endif /* ASSETS_H */
