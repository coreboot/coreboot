/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 * Copyright (C) 2012 Google, Inc.
 * Copyright (C) 2013 The Chromium OS Authors. All rights reserved.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
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
#include <cbfs_serialized.h>

#define CBFS_HEADER_INVALID_ADDRESS	((void*)(0xffffffff))

#define CBFS_NAME(_c) (((char *) (_c)) + sizeof(struct cbfs_file))
#define CBFS_SUBHEADER(_p) ( (void *) ((((uint8_t *) (_p)) + ntohl((_p)->offset))) )

/*
 * ROMCC does not understand uint64_t, so we hide future definitions as they are
 * unlikely to be ever needed from ROMCC
 */
#ifndef __ROMCC__

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
 * Locate file by name and fill in cbfs_file in host byte order. Returns
 * < 0 on error, else the offset of the file data.
 */
ssize_t cbfs_locate_file(struct cbfs_media *media, struct cbfs_file *file,
				const char *name);

/* Read count bytes at offset into dest. Return number of bytes read. */
size_t cbfs_read(struct cbfs_media *media, void *dest, size_t offset,
			size_t count);

/* returns pointer to a file entry inside CBFS or NULL */
struct cbfs_file *cbfs_get_file(struct cbfs_media *media, const char *name);

/* returns pointer to file content inside CBFS after if type is correct */
void *cbfs_get_file_content(struct cbfs_media *media, const char *name,
			    int type, size_t *sz);

/* returns decompressed size on success, 0 on failure */
int cbfs_decompress(int algo, void *src, void *dst, int len);

/* returns a pointer to CBFS master header, or CBFS_HEADER_INVALID_ADDRESS
 *  on failure */
const struct cbfs_header *cbfs_get_header(struct cbfs_media *media);

#endif /* __ROMCC__ */

#endif
