/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 * Copyright (C) 2014 Imagination Technologies
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
#ifndef PROGRAM_LOADING_H
#define PROGRAM_LOADING_H

#include <stdint.h>
#include <stddef.h>

/* For each segment of a program loaded this function is called*/
void arch_program_segment_loaded(uintptr_t start, size_t size);

/* Upon completion of loading a program this function is called */
void arch_program_loaded(void);

/************************
 *   ROMSTAGE LOADING   *
 ************************/

/* Run romstage from bootblock. */
void run_romstage(void);

/************************
 *   RAMSTAGE LOADING   *
 ************************/

struct romstage_handoff;
struct cbmem_entry;

#if defined(__PRE_RAM__)
#if CONFIG_RELOCATABLE_RAMSTAGE
/* The cache_loaded_ramstage() and load_cached_ramstage() functions are defined
 * to be weak so that board and chipset code may override them. Their job is to
 * cache and load the ramstage for quick S3 resume. By default a copy of the
 * relocated ramstage is saved using the cbmem infrastructure. These
 * functions are only valid during romstage. */

/* The implementer of cache_loaded_ramstage() may use the romstage_handoff
 * structure to store information, but note that the handoff variable can be
 * NULL. The ramstage cbmem_entry represents the region occupied by the loaded
 * ramstage. */
void cache_loaded_ramstage(struct romstage_handoff *handoff,
                      const struct cbmem_entry *ramstage, void *entry_point);
/* Return NULL on error or entry point on success. The ramstage cbmem_entry is
 * the region where to load the cached contents to. */
void * load_cached_ramstage(struct romstage_handoff *handoff,
                     const struct cbmem_entry *ramstage);
#else  /* CONFIG_RELOCATABLE_RAMSTAGE */

static inline void cache_loaded_ramstage(struct romstage_handoff *handoff,
			const struct cbmem_entry *ramstage, void *entry_point)
{
}

static inline void *
load_cached_ramstage(struct romstage_handoff *handoff,
			const struct cbmem_entry *ramstage)
{
	return NULL;
}

#endif /* CONFIG_RELOCATABLE_RAMSTAGE */
#endif /* defined(__PRE_RAM__) */

/* Run ramstage from romstage. */
void run_ramstage(void);

struct ramstage_loader_ops {
	const char *name;
	void *(*load)(uint32_t cbmem_id, const char *name,
			const struct cbmem_entry **cbmem_entry);
};

/***********************
 *   PAYLOAD LOADING   *
 ***********************/

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


#endif /* PROGRAM_LOADING_H */
