/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _LIBLAR_H_
#define _LIBLAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lar.h"

typedef void (*LAR_CompFunc) (char *, int, char *, int *);
typedef void (*LAR_DecompFunc) (char *, int, char *, int);

struct LAR {
	int fd;
	LAR_CompFunc cfuncs[ALGO_INVALID];
	LAR_DecompFunc dfuncs[ALGO_INVALID];
};

struct LARHeader {
	u32 len;
	u32 reallen;
	u32 checksum;
	u32 compression;
	u64 loadaddress;
	u64 entry;
	/* These are the offsets within the file. */
	unsigned int offset;
	unsigned int hoffset;
};

struct LARAttr {
	char name[MAX_PATHLEN];
	u64 loadaddr;
	int compression;
	u64 entry;
};

struct LARFile {
	int len;
	char *buffer;
};

int LAR_AppendBuffer(struct LAR *lar, unsigned char *buffer, int len,
		     struct LARAttr *attr);
int LAR_AppendSelf(struct LAR *lar, const char *filename, struct LARAttr *attr);
int LAR_AppendFile(struct LAR *lar, const char *filename, struct LARAttr *attr);
int LAR_DeleteFile(struct LAR *lar, const char *filename);
void LAR_CloseFile(struct LARFile *file);
struct LARFile *LAR_MapFile(struct LAR *lar, const char *filename);
int LAR_SetCompressionFuncs(struct LAR *lar, int algo,
			    LAR_CompFunc cfunc, LAR_DecompFunc dfunc);
void LAR_Close(struct LAR *lar);
struct LAR *LAR_Open(const char *filename);
struct LAR *LAR_Create(const char *filename);
void LAR_SetAttrs(struct LARAttr *attrs, char *name, int algo);

#endif
