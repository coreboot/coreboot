/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STAGE_CACHE_H_
#define _STAGE_CACHE_H_

#include <romstage_handoff.h>
#include <stddef.h>
#include <stdint.h>
#include <program_loading.h>

/* Types of stages that may be stored in stage cache */
enum {
	STAGE_RAMSTAGE,
	STAGE_REFCODE,
	STAGE_POSTCAR,
};

/* Types of raw data that may be stored in stage cache */
enum {
	STAGE_S3_DATA,
};

#if CONFIG(TSEG_STAGE_CACHE) || CONFIG(CBMEM_STAGE_CACHE)
/* Cache the loaded stage provided according to the parameters. */
void stage_cache_add(int stage_id, const struct prog *stage);
/* Load the cached stage at given location returning the stage entry point. */
void stage_cache_load_stage(int stage_id, struct prog *stage);
/* Cache non-specific data or code. */
void stage_cache_add_raw(int stage_id, const void *base, const size_t size);
/* Get a pointer to cached raw data and its size. */
void stage_cache_get_raw(int stage_id, void **base, size_t *size);

#else /* CONFIG_NO_STAGE_CACHE */

static inline void stage_cache_add(int stage_id, const struct prog *stage) {}
static inline void stage_cache_load_stage(int stage_id, struct prog *stage) {}
static inline void stage_cache_add_raw(int stage_id, const void *base, const size_t size) {}
static inline void stage_cache_get_raw(int stage_id, void **base, size_t *size) {}

#endif

static inline bool resume_from_stage_cache(void)
{
	if (CONFIG(NO_STAGE_CACHE))
		return false;

	/* TBD: Replace this with acpi_is_wakeup_s3(). */
	return romstage_handoff_is_resume();
}

/* Fill in parameters for the external stage cache, if utilized. */
void stage_cache_external_region(void **base, size_t *size);

/* Metadata associated with each stage. */
struct stage_cache {
	uint64_t load_addr;
	uint64_t entry_addr;
	uint64_t arg;
};

#endif /* _STAGE_CACHE_H_ */
