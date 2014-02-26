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
#ifndef PAYLOAD_LOADER_H
#define PAYLOAD_LOADER_H

#include <stdint.h>
#include <stddef.h>

struct buffer_area {
	void *data;
	size_t size;
};

struct payload {
	const char *name;
	struct buffer_area backing_store;
	/* Used when payload wants memory coreboot ramstage is running at. */
	struct buffer_area bounce;
	void *entry;
};

/*
 * Load payload into memory and return pointer to payload structure. Returns
 * NULL on error.
 */
struct payload *payload_load(void);

/* Run the loaded payload. */
void payload_run(const struct payload *payload);

/* Mirror the payload to be loaded. */
void mirror_payload(struct payload *payload);

/* architecture specific function to run payload. */
void arch_payload_run(const struct payload *payload);

/* Payload loading operations. */
struct payload_loader_ops {
	const char *name;
	/*
	 * Fill in payload_backing_store structure.  Return 0 on success, < 0
	 * on failure.
	 */
	int (*locate)(struct payload *payload);
};

/* Defined in src/lib/selfboot.c */
void *selfload(struct payload *payload);

#endif  /* PAYLOAD_LOADER_H */
