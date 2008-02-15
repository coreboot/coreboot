/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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

#ifndef LAR_H
#define LAR_H

#include <types.h>
#include <shared.h>

#define MAGIC "LARCHIVE"
#define MAX_PATHLEN 1024
/* NOTE -- This and the user-mode lar.h may NOT be in sync. Be careful. */
struct lar_header {
	char magic[8];
	u32 len;
	u32 reallen;
	u32 checksum;
	u32 compchecksum;
	u32 offset;
	/* Compression:
	 * 0 = no compression
	 * 1 = lzma
	 * 2 = nrv2b
	 * 3 = zeroes
	 */
	u32 compression;
	u64 entry;
	u64 loadaddress;
};

struct mem_file {
	void *start;
	int len;
	u32 reallen;
	u32 compression;
	void *entry;
	void *loadaddress;
};

/* Prototypes. */
/* architecture-defined */
SHARED(init_archive, void, struct mem_file *);
/* architecture-independent */
SHARED(find_file, int, const struct mem_file *archive, const char *filename, struct mem_file *result);
SHARED(copy_file, int, const struct mem_file *archive, const char *filename, void *where);
SHARED(run_file, int, const struct mem_file *archive, const char *filename, void *where);
SHARED(execute_in_place, int, const struct mem_file *archive, const char *filename);
SHARED(run_address, int, void *f);
SHARED(load_file, void *, const struct mem_file *archive, const char *filename);
SHARED(load_file_segments, void *, const struct mem_file *archive, const char *filename);
SHARED(process_file, int, const struct mem_file *archive, void *where);
#endif /* LAR_H */
