/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#ifndef _COMMONLIB_BSD_METADATA_HASH_H_
#define _COMMONLIB_BSD_METADATA_HASH_H_

#include <stdint.h>
#include <vb2_sha.h>

/* This structure is embedded somewhere in the (uncompressed) bootblock. */
struct metadata_hash_anchor {
	uint8_t magic[8];
	struct vb2_hash cbfs_hash;
	/* NOTE: This is just reserving space. sizeof(struct vb2_hash) may change between
	   configurations/versions and cannot be relied upon, so the FMAP hash must be placed
	   right after the actual data for the particular CBFS hash algorithm used ends. */
	uint8_t reserved_space_for_fmap_hash[VB2_MAX_DIGEST_SIZE];
} __packed;

/* Always use this function to figure out the actual location of the FMAP hash. It always uses
   the same algorithm as the CBFS hash. */
static inline uint8_t *metadata_hash_anchor_fmap_hash(struct metadata_hash_anchor *anchor)
{
	return anchor->cbfs_hash.raw + vb2_digest_size(anchor->cbfs_hash.algo);
}

/*
 * Do not use this constant anywhere else in coreboot code to ensure the bit pattern really only
 * appears once in the CBFS image. The only coreboot file allowed to use this is
 * src/lib/metadata_anchor.c to define the actual anchor data structure. It is defined here so
 * that it can be shared with cbfstool (which may use it freely).
 */
#define DO_NOT_USE_METADATA_HASH_ANCHOR_MAGIC_DO_NOT_USE	"\xadMdtHsh\x15"

#endif /* _COMMONLIB_BSD_MASTER_HASH_H_ */
