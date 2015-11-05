/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2015 The ChromiumOS Authors.  All rights reserved.
 *                 written by Daisuke Nojiri <dnojiri@chromium.org>
 *
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
 */

#ifndef __ARCHIVE_H
#define __ARCHIVE_H

#include <stdint.h>

/*
 * Archive file layout:
 *
 *  +----------------------------------+
 *  |           root header            |
 *  +----------------------------------+
 *  |         file_header[0]           |
 *  +----------------------------------+
 *  |         file_header[1]           |
 *  +----------------------------------+
 *  |              ...                 |
 *  +----------------------------------+
 *  |         file_header[count-1]     |
 *  +----------------------------------+
 *  |         file(0) content          |
 *  +----------------------------------+
 *  |         file(1) content          |
 *  +----------------------------------+
 *  |              ...                 |
 *  +----------------------------------+
 *  |         file(count-1) content    |
 *  +----------------------------------+
 */

#define VERSION		0
#define CBAR_MAGIC	"CBAR"
#define NAME_LENGTH	32

/* Root header */
struct directory {
	char magic[4];
	uint32_t version;	/* version of the header. little endian */
	uint32_t size;		/* total size of archive. little endian */
	uint32_t count;		/* number of files. little endian */
};

/* File header */
struct dentry {
	/* file name. null-terminated if shorter than NAME_LENGTH */
	char name[NAME_LENGTH];
	/* file offset from the root header. little endian */
	uint32_t offset;
	/* file size. little endian */
	uint32_t size;
};

static inline struct dentry *get_first_dentry(const struct directory *dir)
{
	return (struct dentry *)(dir + 1);
}

static inline uint32_t get_first_offset(const struct directory *dir)
{
	return sizeof(struct directory) + sizeof(struct dentry) * dir->count;
}

#endif
