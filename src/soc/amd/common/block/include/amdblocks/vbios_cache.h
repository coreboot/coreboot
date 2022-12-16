/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __VBIOS_CACHE_H__
#define __VBIOS_CACHE_H__

#include <types.h>

#define VBIOS_CACHE_FMAP_NAME	"RW_VBIOS_CACHE"
#define VBIOS_CACHE_FMAP_SIZE	 0x10000

/*
 * Return true if VBIOS cache contains valid data
 */
bool vbios_cache_is_valid(void);

/*
 * Loads cached VBIOS data into legacy oprom location.
 */
void vbios_load_from_cache(void);

#endif  /* __VBIOS_CACHE_H__ */
