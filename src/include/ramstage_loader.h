/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
#ifndef RAMSTAGE_LOADER_H
#define RAMSTAGE_LOADER_H

#include <stdint.h>
struct cbmem_entry;

/* Run ramstage from romstage. */
void run_ramstage(void);

struct ramstage_loader_ops {
	const char *name;
	void *(*load)(uint32_t cbmem_id, const char *name,
			const struct cbmem_entry **cbmem_entry);
};

#endif  /* RAMSTAGE_LOADER_H */
