/* Firmware Interface Table support */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CBFSTOOL_FIT_H
#define __CBFSTOOL_FIT_H

#include "cbfs_image.h"
#include "common.h"

/**
 * Based on "Intel Trusted Execution Technology (Intel TXT) LAB Handout" and
 * https://github.com/slimbootloader/slimbootloader/
 */
enum fit_type {
	FIT_TYPE_HEADER = 0,
	FIT_TYPE_MICROCODE = 1,
	FIT_TYPE_BIOS_ACM = 2,
	FIT_TYPE_PLATFORM_BOOT_POLICY = 4,
	FIT_TYPE_BIOS_STARTUP = 7,
	FIT_TYPE_TPM_POLICY = 8,
	FIT_TYPE_BIOS_POLICY = 9,
	FIT_TYPE_TXT_POLICY = 0xa,
	FIT_TYPE_KEY_MANIFEST = 0xb,
	FIT_TYPE_BOOT_POLICY = 0xc,
	FIT_TYPE_CSE_SECURE_BOOT = 0x10,
	FIT_TYPE_TXTSX_POLICY = 0x2d,
	FIT_TYPE_JMP_DEBUG_POLICY = 0x2f,
	FIT_TYPE_UNUSED = 127,
};

/*
 * Converts between offsets from the start of the specified image region and
 * "top-aligned" offsets from the top of the entire flash image. Should work in
 * both directions: accepts either type of offset and produces the other type.
 * The implementation must have some notion of the flash image's total size.
 */
typedef unsigned (*fit_offset_converter_t)(const struct buffer *region,
							unsigned offset);

struct fit_table;

struct fit_table *fit_get_table(struct buffer *bootblock,
				fit_offset_converter_t offset_fn,
				uint32_t topswap_size);
int fit_dump(struct fit_table *fit);
int fit_clear_table(struct fit_table *fit);
int fit_is_supported_type(const enum fit_type type);
int fit_add_entry(struct fit_table *fit,
		  const uint32_t offset,
		  const uint32_t len,
		  const enum fit_type type,
		  const size_t max_fit_entries);
int fit_delete_entry(struct fit_table *fit,
		     const size_t idx);

int fit_add_microcode_file(struct fit_table *fit,
			   struct cbfs_image *image,
			   const char *blob_name,
			   fit_offset_converter_t offset_helper,
			   const size_t max_fit_entries);
int set_fit_pointer(struct buffer *bootblock, const uint32_t offset,
		    fit_offset_converter_t offset_fn, uint32_t topswap_size);
#endif
