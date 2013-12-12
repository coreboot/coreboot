/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _RAMSTAGE_CACHE_
#define _RAMSTAGE_CACHE_

#include <stddef.h>
#include <stdint.h>

/* This structure is saved along with the relocated ramstage program when
 * CONFIG_RELOCATED_RAMSTAGE is employed.  For x86, it can used to protect
 * the integrity of the ramstage program on S3 resume by saving a copy of
 * the relocated ramstage in SMM space with the assumption that the SMM region
 * cannot be altered from the OS. The magic value just serves as a quick sanity
 * check. */

#define RAMSTAGE_CACHE_MAGIC 0xf3c3a02a

struct ramstage_cache {
	uint32_t magic;
	uint32_t entry_point;
	uint32_t load_address;
	uint32_t size;
	char program[0];
} __attribute__((packed));

/* Chipset/Board function for obtaining cache location and size. */
struct ramstage_cache *ramstage_cache_location(long *size);
/* Chipset/Board function called when cache is invalid on resume. */
void ramstage_cache_invalid(struct ramstage_cache *cache);

static inline int ramstage_cache_is_valid(const struct ramstage_cache *c)
{
	return (c != NULL && c->magic == RAMSTAGE_CACHE_MAGIC);
}

#endif  /* _RAMSTAGE_CACHE_ */
