/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#ifndef _ROMFS_H_
#define _ROMFS_H_

/** These are standard values for the known compression
    alogrithms that coreboot knows about for stages and
    payloads.  Of course, other LAR users can use whatever
    values they want, as long as they understand them. */

#define ROMFS_COMPRESS_NONE  0
#define ROMFS_COMPRESS_LZMA  1
#define ROMFS_COMPRESS_NRV2B 2

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define ROMFS_TYPE_STAGE     0x10
#define ROMFS_TYPE_PAYLOAD   0x20
#define ROMFS_TYPE_OPTIONROM 0x30

/** this is the master romfs header - it need to be
    located somewhere in the bootblock.  Where it
    actually lives is up to coreboot. A pointer to
    this header will live at 0xFFFFFFFc, so we can
    easily find it. */

#define ROMFS_HEADER_MAGIC  0x4F524243
#define ROMFS_HEADPTR_ADDR 0xFFFFFFFc
#define VERSION1 0x31313131

struct romfs_header {
	u32 magic;
        u32 version; 
        u32 romsize;
        u32 bootblocksize;
	u32 align;
	u32 offset;
	u32 pad[2];
} __attribute__((packed));

/** This is a component header - every entry in the ROMFS
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

#define ROMFS_FILE_MAGIC "LARCHIVE"

struct romfs_file {
	char magic[8];
	u32 len;
	u32 type;
	u32 checksum;
	u32 offset;
} __attribute__((packed));

/*** Component sub-headers ***/

/* Following are component sub-headers for the "standard"
   component types */

/** This is the sub-header for stage components.  Stages are
    loaded by coreboot during the normal boot process */

struct romfs_stage {
	u32 compression;  /** Compression type */
	u64 entry;  /** entry point */
	u64 load;   /** Where to load in memory */
	u32 len;          /** length of data to load */
	u32 memlen;	   /** total length of object in memory */
} __attribute__((packed));

/** this is the sub-header for payload components.  Payloads
    are loaded by coreboot at the end of the boot process */

struct romfs_payload_segment {
	u32 type;
	u32 compression;
	u32 offset;
	u64 load_addr;
	u32 len;
	u32 mem_len;
} __attribute__((packed));

struct romfs_payload {
	struct romfs_payload_segment segments;
};

#define PAYLOAD_SEGMENT_CODE   0x45444F43
#define PAYLOAD_SEGMENT_DATA   0x41544144
#define PAYLOAD_SEGMENT_BSS    0x20535342
#define PAYLOAD_SEGMENT_PARAMS 0x41524150
#define PAYLOAD_SEGMENT_ENTRY  0x52544E45

struct romfs_optionrom {
	u32 compression;
	u32 len;
} __attribute__((packed));

#define ROMFS_NAME(_c) (((char *) (_c)) + sizeof(struct romfs_file))
#define ROMFS_SUBHEADER(_p) ( (void *) ((((u8 *) (_p)) + ntohl((_p)->offset))) )

void * romfs_load_payload(struct lb_memory *lb_mem, const char *name);
void * romfs_load_stage(const char *name);
int romfs_execute_stage(const char *name);
void * romfs_get_file(const char *name);
void *romfs_load_optionrom(u16 vendor, u16 device, void * dest);
int run_address(void *f);

#endif

