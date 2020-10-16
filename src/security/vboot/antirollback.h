/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#ifndef ANTIROLLBACK_H_
#define ANTIROLLBACK_H_

#include <types.h>
#include <security/tpm/tspi.h>
#include <vb2_sha.h>

struct vb2_context;
enum vb2_pcr_digest;

/* TPM NVRAM location indices. */
#define FIRMWARE_NV_INDEX               0x1007
#define KERNEL_NV_INDEX                 0x1008
/* 0x1009 used to be used as a backup space. Think of conflicts if you
 * want to use 0x1009 for something else. */
#define BACKUP_NV_INDEX                 0x1009
#define FWMP_NV_INDEX                   0x100a
/* 0x100b: Hash of MRC_CACHE training data for recovery boot */
#define MRC_REC_HASH_NV_INDEX           0x100b
#define HASH_NV_SIZE                    VB2_SHA256_DIGEST_SIZE

/* Structure definitions for TPM spaces */

/* Flags for firmware space */

/*
 * Last boot was developer mode.  TPM ownership is cleared when transitioning
 * to/from developer mode.
 */
#define FLAG_LAST_BOOT_DEVELOPER 0x01

/* All functions return TPM_SUCCESS (zero) if successful, non-zero if error */

uint32_t antirollback_read_space_firmware(struct vb2_context *ctx);

/**
 * Write may be called if the versions change.
 */
uint32_t antirollback_write_space_firmware(struct vb2_context *ctx);

/**
 * Read and write kernel space in TPM.
 */
uint32_t antirollback_read_space_kernel(struct vb2_context *ctx);
uint32_t antirollback_write_space_kernel(struct vb2_context *ctx);

/**
 * Lock must be called.
 */
uint32_t antirollback_lock_space_firmware(void);

/*
 * Read recovery hash data from TPM.
 * @param index index into TPM NVRAM where hash is stored
 * @param data  pointer to buffer where hash from TPM read into
 * @param size  size of buffer
 */
uint32_t antirollback_read_space_mrc_hash(uint32_t index, uint8_t *data, uint32_t size);
/*
 * Write new hash data to recovery space in TPM.\
 * @param index index into TPM NVRAM where hash is stored
 * @param data  pointer to buffer of hash value to be written
 * @param size  size of buffer
*/
uint32_t antirollback_write_space_mrc_hash(uint32_t index, const uint8_t *data,
					   uint32_t size);
/*
 * Lock down recovery hash space in TPM.
 * @param index index into TPM NVRAM where hash is stored
*/
uint32_t antirollback_lock_space_mrc_hash(uint32_t index);

#endif  /* ANTIROLLBACK_H_ */
