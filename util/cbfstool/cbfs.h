/*
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2016 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CBFS_H
#define __CBFS_H

#include "common.h"
#include <stdint.h>

#include <vb2_api.h>

/* cbfstool will fail when trying to build a cbfs_file header that's larger
 * than MAX_CBFS_FILE_HEADER_BUFFER. 1K should give plenty of room. */
#define MAX_CBFS_FILE_HEADER_BUFFER 1024

/* create a magic number in host-byte order.
 * b3 is the high order byte.
 * in the coreboot tools, we go with the 32-bit
 * magic number convention.
 * This was an inline func but that breaks anything
 * that uses it in a case statement.
 */

#define makemagic(b3, b2, b1, b0)\
	(((b3)<<24) | ((b2) << 16) | ((b1) << 8) | (b0))

#if defined(__WIN32) || defined(__WIN64)
#define __PACKED __attribute__((gcc_struct, packed))
#else
#define __PACKED __attribute__((packed))
#endif

/* To make CBFS more friendly to ROM, fill -1 (0xFF) instead of zero. */
#define CBFS_CONTENT_DEFAULT_VALUE	(-1)

// Alignment (in bytes) to be used when no master header is present
#define CBFS_ENTRY_ALIGNMENT 64

#define CBFS_HEADER_MAGIC  0x4F524243
#define CBFS_HEADPTR_ADDR_X86 0xFFFFFFFC
#define CBFS_HEADER_VERSION1 0x31313131
#define CBFS_HEADER_VERSION2 0x31313132
#define CBFS_HEADER_VERSION  CBFS_HEADER_VERSION2

#define CBFS_ALIGNMENT 64

struct cbfs_header {
	uint32_t magic;
	uint32_t version;
	uint32_t romsize;
	uint32_t bootblocksize;
	uint32_t align; /* hard coded to 64 byte */
	uint32_t offset;
	uint32_t architecture;	/* Version 2 */
	uint32_t pad[1];
} __PACKED;

#define CBFS_ARCHITECTURE_UNKNOWN  0xFFFFFFFF
#define CBFS_ARCHITECTURE_X86      0x00000001
#define CBFS_ARCHITECTURE_ARM      0x00000010
#define CBFS_ARCHITECTURE_AARCH64  0x0000aa64
#define CBFS_ARCHITECTURE_MIPS     0x00000100
#define CBFS_ARCHITECTURE_RISCV    0xc001d0de
#define CBFS_ARCHITECTURE_PPC64    0x407570ff

#define CBFS_FILE_MAGIC "LARCHIVE"

struct cbfs_file {
	uint8_t magic[8];
	/* length of file data */
	uint32_t len;
	uint32_t type;
	/* offset to struct cbfs_file_attribute or 0 */
	uint32_t attributes_offset;
	/* length of header incl. variable data */
	uint32_t offset;
	char filename[];
} __PACKED;

#if defined __GNUC__ && (__GNUC__ * 100 + __GNUC_MINOR__) >= 406
_Static_assert(sizeof(struct cbfs_file) == 24, "cbfs_file size mismatch");
#endif

/* The common fields of extended cbfs file attributes.
   Attributes are expected to start with tag/len, then append their
   specific fields. */
struct cbfs_file_attribute {
	uint32_t tag;
	/* len covers the whole structure, incl. tag and len */
	uint32_t len;
	uint8_t data[0];
} __PACKED;

/* Depending on how the header was initialized, it may be backed with 0x00 or
 * 0xff. Support both. */
#define CBFS_FILE_ATTR_TAG_UNUSED 0
#define CBFS_FILE_ATTR_TAG_UNUSED2 0xffffffff
#define CBFS_FILE_ATTR_TAG_COMPRESSION 0x42435a4c
#define CBFS_FILE_ATTR_TAG_HASH 0x68736148
#define CBFS_FILE_ATTR_TAG_POSITION 0x42435350  /* PSCB */
#define CBFS_FILE_ATTR_TAG_ALIGNMENT 0x42434c41 /* ALCB */

struct cbfs_file_attr_compression {
	uint32_t tag;
	uint32_t len;
	/* whole file compression format. 0 if no compression. */
	uint32_t compression;
	uint32_t decompressed_size;
} __PACKED;

struct cbfs_file_attr_hash {
	uint32_t tag;
	uint32_t len;
	uint32_t hash_type;
	/* hash_data is len - sizeof(struct) bytes */
	uint8_t  hash_data[];
} __PACKED;

struct cbfs_file_attr_position {
	uint32_t tag;
	uint32_t len;
	uint32_t position;
} __PACKED;

struct cbfs_file_attr_align {
	uint32_t tag;
	uint32_t len;
	uint32_t alignment;
} __PACKED;

struct cbfs_stage {
	uint32_t compression;
	uint64_t entry;
	uint64_t load;
	uint32_t len;
	uint32_t memlen;
} __PACKED;

#define PAYLOAD_SEGMENT_CODE	makemagic('C', 'O', 'D', 'E')
#define PAYLOAD_SEGMENT_DATA	makemagic('D', 'A', 'T', 'A')
#define PAYLOAD_SEGMENT_BSS	makemagic('B', 'S', 'S', ' ')
#define PAYLOAD_SEGMENT_PARAMS	makemagic('P', 'A', 'R', 'A')
#define PAYLOAD_SEGMENT_ENTRY	makemagic('E', 'N', 'T', 'R')

struct cbfs_payload_segment {
	uint32_t type;
	uint32_t compression;
	uint32_t offset;
	uint64_t load_addr;
	uint32_t len;
	uint32_t mem_len;
} __PACKED;

struct cbfs_payload {
	struct cbfs_payload_segment segments;
} __PACKED;

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define CBFS_COMPONENT_BOOTBLOCK  0x01
#define CBFS_COMPONENT_CBFSHEADER 0x02
#define CBFS_COMPONENT_STAGE      0x10
#define CBFS_COMPONENT_PAYLOAD    0x20
#define CBFS_COMPONENT_OPTIONROM  0x30
#define CBFS_COMPONENT_BOOTSPLASH 0x40
#define CBFS_COMPONENT_RAW        0x50
#define CBFS_COMPONENT_VSA        0x51
#define CBFS_COMPONENT_MBI        0x52
#define CBFS_COMPONENT_MICROCODE  0x53
#define CBFS_COMPONENT_FSP        0x60
#define CBFS_COMPONENT_MRC        0x61
#define CBFS_COMPONENT_MMA	  0x62
#define CBFS_COMPONENT_EFI	  0x63
#define CBFS_COMPONENT_STRUCT	  0x70
#define CBFS_COMPONENT_CMOS_DEFAULT 0xaa
#define CBFS_COMPONENT_SPD          0xab
#define CBFS_COMPONENT_MRC_CACHE    0xac
#define CBFS_COMPONENT_CMOS_LAYOUT 0x01aa

/* The deleted type is chosen to be a value
 * that can be written in a FLASH from all other
 * values.
 */
#define CBFS_COMPONENT_DELETED 0

/* for all known FLASH, this value can be changed
 * to all other values. This allows NULL files to be
 * changed without a block erase
 */
#define CBFS_COMPONENT_NULL 0xFFFFFFFF

struct typedesc_t {
	uint32_t type;
	const char *name;
};

static struct typedesc_t filetypes[] unused = {
	{CBFS_COMPONENT_BOOTBLOCK, "bootblock"},
	{CBFS_COMPONENT_CBFSHEADER, "cbfs header"},
	{CBFS_COMPONENT_STAGE, "stage"},
	{CBFS_COMPONENT_PAYLOAD, "payload"},
	{CBFS_COMPONENT_OPTIONROM, "optionrom"},
	{CBFS_COMPONENT_BOOTSPLASH, "bootsplash"},
	{CBFS_COMPONENT_RAW, "raw"},
	{CBFS_COMPONENT_VSA, "vsa"},
	{CBFS_COMPONENT_MBI, "mbi"},
	{CBFS_COMPONENT_MICROCODE, "microcode"},
	{CBFS_COMPONENT_FSP, "fsp"},
	{CBFS_COMPONENT_MRC, "mrc"},
	{CBFS_COMPONENT_CMOS_DEFAULT, "cmos_default"},
	{CBFS_COMPONENT_CMOS_LAYOUT, "cmos_layout"},
	{CBFS_COMPONENT_SPD, "spd"},
	{CBFS_COMPONENT_MRC_CACHE, "mrc_cache"},
	{CBFS_COMPONENT_MMA, "mma"},
	{CBFS_COMPONENT_EFI, "efi"},
	{CBFS_COMPONENT_STRUCT, "struct"},
	{CBFS_COMPONENT_DELETED, "deleted"},
	{CBFS_COMPONENT_NULL, "null"}
};

static const struct typedesc_t types_cbfs_hash[] unused = {
	{VB2_HASH_INVALID, "none"},
	{VB2_HASH_SHA1, "sha1"},
	{VB2_HASH_SHA256, "sha256"},
	{VB2_HASH_SHA512, "sha512"},
	{0, NULL}
};

static size_t widths_cbfs_hash[] unused = {
	[VB2_HASH_INVALID] = 0,
	[VB2_HASH_SHA1] = 20,
	[VB2_HASH_SHA256] = 32,
	[VB2_HASH_SHA512] = 64,
};

#define CBFS_NUM_SUPPORTED_HASHES ARRAY_SIZE(widths_cbfs_hash)

#define CBFS_SUBHEADER(_p) ( (void *) ((((uint8_t *) (_p)) + ntohl((_p)->offset))) )

/* cbfs_image.c */
uint32_t get_cbfs_entry_type(const char *name, uint32_t default_value);
uint32_t get_cbfs_compression(const char *name, uint32_t unknown);

/* cbfs-mkpayload.c */
void xdr_segs(struct buffer *output,
	      struct cbfs_payload_segment *segs, int nseg);
void xdr_get_seg(struct cbfs_payload_segment *out,
		struct cbfs_payload_segment *in);

#endif
