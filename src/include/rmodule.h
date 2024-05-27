/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef RMODULE_H
#define RMODULE_H

#include <stdint.h>
#include <stddef.h>
#include <commonlib/rmodule-defs.h>

enum {
	RMODULE_TYPE_SMM,
	RMODULE_TYPE_SIPI_VECTOR,
	RMODULE_TYPE_STAGE,
	RMODULE_TYPE_VBOOT,
};

struct rmodule;

/* Public API for loading rmodules. */
int rmodule_parse(void *ptr, struct rmodule *m);
void *rmodule_parameters(const struct rmodule *m);
void *rmodule_entry(const struct rmodule *m);
int rmodule_entry_offset(const struct rmodule *m);
int rmodule_memory_size(const struct rmodule *m);
int rmodule_load(void *loc, struct rmodule *m);
int rmodule_load_alignment(const struct rmodule *m);
/* rmodule_calc_region() calculates the region size, offset to place an
 * rmodule in memory, and load address offset based off of a region allocator
 * with an alignment of region_alignment. This function helps place an rmodule
 * in the same location in RAM it will run from. The offset to place the
 * rmodule into the region allocated of size region_size is returned. The
 * load_offset is the address to load and relocate the rmodule.
 * region_alignment must be a power of 2. */
int rmodule_calc_region(unsigned int region_alignment, size_t rmodule_size,
			size_t *region_size, int *load_offset);

/* Support for loading rmodule stages. This API is only available when
 * using dynamic cbmem because it uses the dynamic cbmem API to obtain
 * the backing store region for the stage. */
struct prog;

struct rmod_stage_load {
	uint32_t cbmem_id;
	struct prog *prog;
	void *params;
};

/* Both of the following functions return 0 on success, -1 on error. */
int rmodule_stage_load(struct rmod_stage_load *rsl);

struct rmodule {
	void *location;
	struct rmodule_header *header;
	const void *payload;
	int payload_size;
	void *relocations;
};

#if CONFIG(RELOCATABLE_MODULES)
/* Rmodules have an entry point of named _start. */
#define RMODULE_ENTRY(entry_) \
	void _start(void *) __attribute__((alias(STRINGIFY(entry_))))
#else
#define RMODULE_ENTRY(entry_)
#endif

#endif /* RMODULE_H */
