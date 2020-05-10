/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef ARCH_CACHE_H
#define ARCH_CACHE_H

#include <cpu/x86/cache.h>

/* Executing WBINVD when running out of CAR would not be good, prevent that. */
static inline void dcache_clean_invalidate_all(void)
{
	if (!ENV_CACHE_AS_RAM)
		wbinvd();
}
static inline void dcache_clean_all(void)
{
	/* x86 doesn't have a "clean without invalidate", fall back to both. */
	dcache_clean_invalidate_all();
}
static inline void dcache_invalidate_all(void)
{
	if (!ENV_CACHE_AS_RAM)
		invd();
}

#endif /* ARCH_CACHE_H */
