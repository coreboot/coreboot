#ifndef __BDK_TRUST_H__
#define __BDK_TRUST_H__
/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/**
 * @file
 *
 * Master include file for trusted boot support. Use bdk.h instead
 * of including this file directly.
 *
 * @defgroup trust Trusted boot support
 */

#include "bdk-signed.h"

typedef enum
{
    BDK_TRUST_LEVEL_BROKEN,     /* Trust is unknown or was broken during boot. Fatal error state */
    BDK_TRUST_LEVEL_NONE,       /* Untrusted boot */
    BDK_TRUST_LEVEL_SIGNED,     /* Trusted boot verified by ROTPK */
    BDK_TRUST_LEVEL_SIGNED_SSK, /* Trusted boot with SSK encryption */
    BDK_TRUST_LEVEL_SIGNED_BSSK,/* Trusted boot with BSSK encryption */
}
bdk_trust_level_t;

typedef struct
{
    uint64_t total_length;
    uint32_t s[8];
} bdk_sha256_state_t;

/**
 * Start a new SHA256
 *
 * @param hash_state Hash state to initialize
 */
extern void bdk_sha256_init(bdk_sha256_state_t *hash_state);

/**
 * Update SHA256 for a data block
 *
 * @param hash_state Hash state
 * @param data       Data to hash
 * @param size       Size of the data in bytes
 */
extern void bdk_sha256_update(bdk_sha256_state_t *hash_state, const void *data, int size);

/**
 * Finish a SHA256
 *
 * @param hash_state Hash state
 *
 * @return Pointer to the 64 byte SHA256
 */
extern void *bdk_sha256_finish(bdk_sha256_state_t *hash_state);

/**
 * Perform AES128 encryption with CBC
 *
 * @param key    Key to use for encryption. Should be a pointer to key memory.
 * @param data   Data to encrypt
 * @param size   Size of the data in bytes. Must be a multiple of 16
 * @param iv     Initial vector. Set to 16 zero bytes for start, then use to chain multiple
 *               calls.
 */
extern void bdk_aes128cbc_encrypt(const void *key, void *data, int size, void *iv);

/**
 * Perform AES128 decryption with CBC
 *
 * @param key    Key to use for decryption. Should be a pointer to key memory.
 * @param data   Data to decrypt
 * @param size   Size of the data in bytes. Must be a multiple of 16
 * @param iv     Initial vector. Set to 16 zero bytes for start, then use to chain multiple
 *               calls.
 */
extern void bdk_aes128cbc_decrypt(const void *key, void *data, int size, void *iv);

/**
 * Called by boot stub (TBL1FW) to initialize the state of trust
 */
extern void __bdk_trust_init(void);

/**
 * Returns the current level of trust. Must be called after
 * __bdk_trust_init()
 *
 * @return Enumerated trsut level, see bdk_trust_level_t
 */
extern bdk_trust_level_t bdk_trust_get_level(void);

/**
 * Return the current secure NV counter stored in the fuses
 *
 * @return NV counter (0-31)
 */
extern int bdk_trust_get_nv_counter(void);

#endif
