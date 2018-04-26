/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 * Copyright (C) 2012 Google, Inc.
 *
 * This file is dual-licensed. You can choose between:
 *   - The GNU GPL, version 2, as published by the Free Software Foundation
 *   - The revised BSD license (without advertising clause)
 *
 * ---------------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ---------------------------------------------------------------------------
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ---------------------------------------------------------------------------
 */

#ifndef _CBFS_CORE_H_
#define _CBFS_CORE_H_

#include <endian.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <compiler.h>

/** These are standard values for the known compression
    alogrithms that coreboot knows about for stages and
    payloads.  Of course, other CBFS users can use whatever
    values they want, as long as they understand them. */

#define CBFS_COMPRESS_NONE  0
#define CBFS_COMPRESS_LZMA  1
#define CBFS_COMPRESS_LZ4   2

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define CBFS_TYPE_STAGE      0x10
#define CBFS_TYPE_SELF       0x20
#define CBFS_TYPE_FIT        0x21
#define CBFS_TYPE_OPTIONROM  0x30
#define CBFS_TYPE_BOOTSPLASH 0x40
#define CBFS_TYPE_RAW        0x50
#define CBFS_TYPE_VSA        0x51
#define CBFS_TYPE_MBI        0x52
#define CBFS_TYPE_MICROCODE  0x53
#define CBFS_TYPE_STRUCT     0x70
#define CBFS_COMPONENT_CMOS_DEFAULT 0xaa
#define CBFS_COMPONENT_CMOS_LAYOUT 0x01aa

#define CBFS_HEADER_MAGIC  0x4F524243
#define CBFS_HEADER_VERSION1 0x31313131
#define CBFS_HEADER_VERSION2 0x31313132
#define CBFS_HEADER_VERSION  CBFS_HEADER_VERSION2

#define CBFS_HEADER_INVALID_ADDRESS	((void*)(0xffffffff))

/* this is the master cbfs header - it must be located somewhere available
 * to bootblock (to load romstage). The last 4 bytes in the image contain its
 * relative offset from the end of the image (as a 32-bit signed integer). */

struct cbfs_header {
	uint32_t magic;
	uint32_t version;
	uint32_t romsize;
	uint32_t bootblocksize;
	uint32_t align; /* fixed to 64 bytes */
	uint32_t offset;
	uint32_t architecture;
	uint32_t pad[1];
} __packed;

/* this used to be flexible, but wasn't ever set to something different. */
#define CBFS_ALIGNMENT 64

/* "Unknown" refers to CBFS headers version 1,
 * before the architecture was defined (i.e., x86 only).
 */
#define CBFS_ARCHITECTURE_UNKNOWN  0xFFFFFFFF
#define CBFS_ARCHITECTURE_X86      0x00000001
#define CBFS_ARCHITECTURE_ARM      0x00000010
#define CBFS_ARCHITECTURE_ARM64    0x00000011

/** This is a component header - every entry in the CBFS
    will have this header.

    This is how the component is arranged in the ROM:

    --------------   <- 0
    component header
    --------------   <- sizeof(struct component)
    component name
    --------------   <- offset
    data
    ...
    --------------   <- offset + len
*/

#define CBFS_FILE_MAGIC "LARCHIVE"

struct cbfs_file {
	char magic[8];
	uint32_t len;
	uint32_t type;
	uint32_t attributes_offset;
	uint32_t offset;
	char filename[];
} __packed;

/* Depending on how the header was initialized, it may be backed with 0x00 or
 * 0xff. Support both. */
#define CBFS_FILE_ATTR_TAG_UNUSED 0
#define CBFS_FILE_ATTR_TAG_UNUSED2 0xffffffff
#define CBFS_FILE_ATTR_TAG_COMPRESSION 0x42435a4c
#define CBFS_FILE_ATTR_TAG_HASH 0x68736148

/* The common fields of extended cbfs file attributes.
   Attributes are expected to start with tag/len, then append their
   specific fields. */
struct cbfs_file_attribute {
	uint32_t tag;
	/* len covers the whole structure, incl. tag and len */
	uint32_t len;
	uint8_t data[0];
} __packed;

struct cbfs_file_attr_compression {
	uint32_t tag;
	uint32_t len;
	/* whole file compression format. 0 if no compression. */
	uint32_t compression;
	uint32_t decompressed_size;
} __packed;

struct cbfs_file_attr_hash {
	uint32_t tag;
	uint32_t len;
	uint32_t hash_type;
	/* hash_data is len - sizeof(struct) bytes */
	uint8_t  hash_data[];
} __packed;

/*** Component sub-headers ***/

/* Following are component sub-headers for the "standard"
   component types */

/** This is the sub-header for stage components.  Stages are
    loaded by coreboot during the normal boot process */

struct cbfs_stage {
	uint32_t compression;  /** Compression type */
	uint64_t entry;  /** entry point */
	uint64_t load;   /** Where to load in memory */
	uint32_t len;          /** length of data to load */
	uint32_t memlen;	   /** total length of object in memory */
} __packed;

/** this is the sub-header for payload components.  Payloads
    are loaded by coreboot at the end of the boot process */

struct cbfs_payload_segment {
	uint32_t type;
	uint32_t compression;
	uint32_t offset;
	uint64_t load_addr;
	uint32_t len;
	uint32_t mem_len;
} __packed;

struct cbfs_payload {
	struct cbfs_payload_segment segments;
};

#define PAYLOAD_SEGMENT_CODE   0x45444F43
#define PAYLOAD_SEGMENT_DATA   0x41544144
#define PAYLOAD_SEGMENT_BSS    0x20535342
#define PAYLOAD_SEGMENT_PARAMS 0x41524150
#define PAYLOAD_SEGMENT_ENTRY  0x52544E45

struct cbfs_optionrom {
	uint32_t compression;
	uint32_t len;
} __packed;

#define CBFS_MEDIA_INVALID_MAP_ADDRESS	((void*)(0xffffffff))
#define CBFS_DEFAULT_MEDIA		((void*)(0x0))

/* Media for CBFS to load files. */
struct cbfs_media {

	/* implementation dependent context, to hold resource references */
	void *context;

	/* opens media and returns 0 on success, -1 on failure */
	int (*open)(struct cbfs_media *media);

	/* returns number of bytes read from media into dest, starting from
	 * offset for count of bytes */
	size_t (*read)(struct cbfs_media *media, void *dest, size_t offset,
		       size_t count);

	/* returns a pointer to memory with count of bytes from media source
	 * starting from offset, or CBFS_MEDIA_INVALID_MAP_ADDRESS on failure.
	 * Note: mapped data can't be free unless unmap is called, even if you
	 * do close first. */
	void * (*map)(struct cbfs_media *media, size_t offset, size_t count);

	/* returns NULL and releases the memory by address, which was allocated
	 * by map */
	void * (*unmap)(struct cbfs_media *media, const void *address);

	/* closes media and returns 0 on success, -1 on failure. */
	int (*close)(struct cbfs_media *media);
};

/*
 * Returns pointer to a copy of the file content or NULL on error.
 * If the file is compressed, data will be decompressed.
 * The caller owns the returned memory.
 */
void *cbfs_get_file_content(struct cbfs_media *media, const char *name,
			    int type, size_t *sz);

/* returns decompressed size on success, 0 on failure */
int cbfs_decompress(int algo, void *src, void *dst, int len);

/* returns a pointer to CBFS master header, or CBFS_HEADER_INVALID_ADDRESS
 *  on failure */
const struct cbfs_header *cbfs_get_header(struct cbfs_media *media);

/* Persistent handle to a CBFS file that has not yet been fully mapped. */
struct cbfs_handle {
	struct cbfs_media media;	/* copy of original media object */
	u32 type;			/* CBFS file type */
	u32 media_offset;		/* offset from beginning of media */
	u32 attribute_offset;		/* relative offset of attributes */
	u32 content_offset;		/* relative offset of contents */
	u32 content_size;		/* length of file contents in bytes */
};

/* Returns handle to CBFS file, or NULL on error. Does not yet map contents.
 * Caller is responsible to free() returned handle after use. */
struct cbfs_handle *cbfs_get_handle(struct cbfs_media *media, const char *name);

/* Given a cbfs_handle and an attribute tag, return a mapping for the first
 * instance of the attribute or NULL if none found. */
void *cbfs_get_attr(struct cbfs_handle *handle, uint32_t tag);

/* Given a cbfs_handle, returns the (decompressed) file contents in a buffer,
 * or NULL on error. If |size| is passed, will store amount of bytes read there.
 * If |limit| is not 0, will only return up to that many bytes. */
void *cbfs_get_contents(struct cbfs_handle *handle, size_t *size, size_t limit);

#endif
