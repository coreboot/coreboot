/*
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

#include <commonlib/bsd/cbfs_serialized.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define CBFS_HEADER_INVALID_ADDRESS	((void*)(0xffffffff))

struct cbfs_stage {
	uint32_t compression;  /** Compression type */
	uint64_t entry;  /** entry point */
	uint64_t load;   /** Where to load in memory */
	uint32_t len;          /** length of data to load */
	uint32_t memlen;	   /** total length of object in memory */
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

/* Returns decompressed size on success, 0 on failure. */
size_t cbfs_decompress(int algo, const void *src, size_t srcn, void *dst,
		       size_t dstn);

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
