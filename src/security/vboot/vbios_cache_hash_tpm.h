/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _VBIOS_CACHE_HASH_TPM_H_
#define _VBIOS_CACHE_HASH_TPM_H_

#include <types.h>

/*
 * Updates vbios cache hash.
 */
void vbios_cache_update_hash(const uint8_t *data, size_t size);

/*
 * Verifies vbios cache hash which is stored in FMAP region.
 */
enum cb_err vbios_cache_verify_hash(const uint8_t *data, size_t size);

#endif /* _VBIOS_CACHE_HASH_TPM_H_ */
