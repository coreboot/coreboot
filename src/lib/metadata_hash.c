/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <metadata_hash.h>
#include <security/vboot/misc.h>
#include <symbols.h>

#if !CONFIG(COMPRESS_BOOTBLOCK) || ENV_DECOMPRESSOR
__attribute__((used, section(".metadata_hash_anchor")))
static struct metadata_hash_anchor metadata_hash_anchor = {
	/* This is the only place in all of coreboot where we actually need to use this. */
	.magic = DO_NOT_USE_METADATA_HASH_ANCHOR_MAGIC_DO_NOT_USE,
	.cbfs_hash = { .algo = CONFIG_CBFS_HASH_ALGO }
};

static struct metadata_hash_anchor *get_anchor(void)
{
	return &metadata_hash_anchor;
}

void *metadata_hash_export_anchor(void)
{
	return get_anchor();
}
#else
static struct metadata_hash_anchor *anchor_ptr = NULL;

static struct metadata_hash_anchor *get_anchor(void)
{
	assert(anchor_ptr != NULL);
	return anchor_ptr;
}

void metadata_hash_import_anchor(void *ptr)
{
	anchor_ptr = ptr;
}
#endif

struct vb2_hash *metadata_hash_get(void)
{
	return &get_anchor()->cbfs_hash;
}

vb2_error_t metadata_hash_verify_fmap(const void *fmap_buffer, size_t fmap_size)
{
	struct vb2_hash hash = { .algo = get_anchor()->cbfs_hash.algo };
	memcpy(hash.raw, metadata_hash_anchor_fmap_hash(get_anchor()),
	       vb2_digest_size(hash.algo));
	return vb2_hash_verify(vboot_hwcrypto_allowed(), fmap_buffer, fmap_size, &hash);
}
