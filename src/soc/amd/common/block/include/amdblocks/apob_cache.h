/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_APOB_CACHE_H
#define AMD_BLOCK_APOB_CACHE_H

/* Start loading the APOB as soon as possible so it is ready by the time we need it. */
void start_apob_cache_read(void);
void *soc_fill_apob_cache(void);

#endif /* AMD_BLOCK_APOB_CACHE_H */
