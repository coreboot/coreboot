/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CACHE_H__
#define __ARCH_CACHE_H__

/* No support for cache in the mock architecture */

#define dmb()
#define dsb()
#define dcache_clean_all()
#define dcache_clean_by_mva(addr, len)
#define dcache_invalidate_all()
#define dcache_invalidate_by_mva(addr, len)
#define dcache_clean_invalidate_all()
#define dcache_clean_invalidate_by_mva(addr, len)
#define cache_sync_instructions()

#endif /* __ARCH_CACHE_H__ */
