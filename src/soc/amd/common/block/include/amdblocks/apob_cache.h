/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_APOB_CACHE_H
#define AMD_BLOCK_APOB_CACHE_H

#include <stddef.h>
#include <stdint.h>

#define APOB_SIGNATURE 0x424F5041	/* 'APOB' */

/* APOB_BASE_HEADER from AGESA */
struct apob_base_header {
	uint32_t   signature;			/* APOB signature */
	uint32_t   version;			/* Version */
	uint32_t   size;			/* APOB Size */
	uint32_t   offset_of_first_entry;	/* APOB Header Size */
};

#if CONFIG(SOC_AMD_COMMON_BLOCK_APOB) && !CONFIG(SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE)
/* Start loading the APOB as soon as possible so it is ready by the time we need it. */
void start_apob_cache_read(void);
void *soc_fill_apob_cache(void);
#else /* CONFIG(SOC_AMD_COMMON_BLOCK_APOB) && !CONFIG(SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE) */
static inline void start_apob_cache_read(void) {}
static inline void *soc_fill_apob_cache(void) { return NULL; }
#endif /* CONFIG(SOC_AMD_COMMON_BLOCK_APOB) && !CONFIG(SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE) */

#endif /* AMD_BLOCK_APOB_CACHE_H */
