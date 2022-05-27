/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CBFS_H
#define __CBFS_H

#include "common.h"
#include <commonlib/bsd/cbfs_serialized.h>

/* To make CBFS more friendly to ROM, fill -1 (0xFF) instead of zero. */
#define CBFS_CONTENT_DEFAULT_VALUE	(-1)

#define CBFS_HEADPTR_ADDR_X86 0xFFFFFFFC

/* cbfstool is allowed to use this constant freely since it's not part of the
   CBFS image, so make an alias for the name that's a little less aggressive. */
#define METADATA_HASH_ANCHOR_MAGIC \
	DO_NOT_USE_METADATA_HASH_ANCHOR_MAGIC_DO_NOT_USE

struct typedesc_t {
	uint32_t type;
	const char *name;
};

static const struct typedesc_t types_cbfs_compression[] = {
	{CBFS_COMPRESS_NONE, "none"},
	{CBFS_COMPRESS_LZMA, "LZMA"},
	{CBFS_COMPRESS_LZ4, "LZ4"},
	{0, NULL},
};

static struct typedesc_t filetypes[] unused = {
	{CBFS_TYPE_BOOTBLOCK, "bootblock"},
	{CBFS_TYPE_CBFSHEADER, "cbfs header"},
	{CBFS_TYPE_STAGE, "stage"},
	{CBFS_TYPE_SELF, "simple elf"},
	{CBFS_TYPE_FIT_PAYLOAD, "fit_payload"},
	{CBFS_TYPE_OPTIONROM, "optionrom"},
	{CBFS_TYPE_BOOTSPLASH, "bootsplash"},
	{CBFS_TYPE_RAW, "raw"},
	{CBFS_TYPE_VSA, "vsa"},
	{CBFS_TYPE_MBI, "mbi"},
	{CBFS_TYPE_MICROCODE, "microcode"},
	{CBFS_TYPE_FSP, "fsp"},
	{CBFS_TYPE_MRC, "mrc"},
	{CBFS_TYPE_CMOS_DEFAULT, "cmos_default"},
	{CBFS_TYPE_CMOS_LAYOUT, "cmos_layout"},
	{CBFS_TYPE_SPD, "spd"},
	{CBFS_TYPE_MRC_CACHE, "mrc_cache"},
	{CBFS_TYPE_MMA, "mma"},
	{CBFS_TYPE_EFI, "efi"},
	{CBFS_TYPE_STRUCT, "struct"},
	{CBFS_TYPE_DELETED, "deleted"},
	{CBFS_TYPE_NULL, "null"},
	{0, NULL}
};

#define CBFS_SUBHEADER(_p) ((void *) ((((uint8_t *) (_p)) + be32toh((_p)->offset))))

static inline size_t cbfs_file_attr_hash_size(enum vb2_hash_algorithm algo)
{
	return offsetof(struct cbfs_file_attr_hash, hash.raw) +
	       vb2_digest_size(algo);
}

/* cbfs_image.c */
uint32_t get_cbfs_entry_type(const char *name, uint32_t default_value);
uint32_t get_cbfs_compression(const char *name, uint32_t unknown);

/* cbfs-mkpayload.c */
void xdr_segs(struct buffer *output,
	      struct cbfs_payload_segment *segs, int nseg);
void xdr_get_seg(struct cbfs_payload_segment *out,
		struct cbfs_payload_segment *in);

/* platform_fixups.c */
typedef int (*platform_fixup_func)(struct buffer *buffer, size_t offset);
platform_fixup_func platform_fixups_probe(struct buffer *buffer, size_t offset,
					  const char *region_name);

#endif
