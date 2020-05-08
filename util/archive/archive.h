/* SPDX-License-Identifier: GPL-2.0-only */

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
