/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CACHE_H__
#define __ARCH_CACHE_H__

#include <stddef.h>

/* Functions in this file are unimplemented by default. Tests are expected to implement
   mocks for these functions, if tests will call functions using functions listed below. */

void dmb(void);
void dsb(void);
void dcache_clean_all(void);
void dcache_clean_by_mva(void const *addr, size_t len);
void dcache_invalidate_all(void);
void dcache_invalidate_by_mva(void const *addr, size_t len);
void dcache_clean_invalidate_all(void);
void dcache_clean_invalidate_by_mva(void const *addr, size_t len);
void cache_sync_instructions(void);

#endif /* __ARCH_CACHE_H__ */
