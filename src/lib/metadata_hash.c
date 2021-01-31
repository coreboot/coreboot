/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <assert.h>
#include <metadata_hash.h>
#include <symbols.h>

__attribute__((used, section(".metadata_hash_anchor")))
static struct metadata_hash_anchor metadata_hash_anchor = {
	/* This is the only place in all of coreboot where we actually need to use this. */
	.magic = DO_NOT_USE_METADATA_HASH_ANCHOR_MAGIC_DO_NOT_USE,
	.cbfs_hash = { .algo = CONFIG_CBFS_HASH_ALGO }
};

struct vb2_hash *metadata_hash_get(void)
{
	return &metadata_hash_anchor.cbfs_hash;
}

vb2_error_t metadata_hash_verify_fmap(const void *fmap_buffer, size_t fmap_size)
{
	struct vb2_hash hash = { .algo = metadata_hash_anchor.cbfs_hash.algo };
	memcpy(hash.raw, metadata_hash_anchor_fmap_hash(&metadata_hash_anchor),
	       vb2_digest_size(hash.algo));
	return vb2_hash_verify(fmap_buffer, fmap_size, &hash);
}
