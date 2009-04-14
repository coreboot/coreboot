/*
 * cbfstool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 *
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
 */

#ifndef _CBFS_H_
#define _CBFS_H_

/** These are standard values for the known compression
    alogrithms that coreboot knows about for stages and
    payloads.  Of course, other LAR users can use whatever
    values they want, as long as they understand them. */

#define CBFS_COMPRESS_NONE  0
#define CBFS_COMPRESS_LZMA  1
#define CBFS_COMPRESS_NRV2B 2

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define CBFS_COMPONENT_STAGE     0x10
#define CBFS_COMPONENT_PAYLOAD   0x20
#define CBFS_COMPONENT_OPTIONROM 0x30

#define CBFS_COMPONENT_NULL 0xFFFFFFFF

/** this is the master cbfs header - it need to be
    located somewhere in the bootblock.  Where it
    actually lives is up to coreboot. A pointer to
    this header will live at 0xFFFFFFF4, so we can
    easily find it. */

#define HEADER_MAGIC 0x4F524243

/* this is a version that gives the right answer in any endian-ness */
#define VERSION1 0x31313131

struct cbfs_header {
	unsigned int magic;
	unsigned int version; 
	unsigned int romsize;
	unsigned int bootblocksize;
	unsigned int align;
	unsigned int offset;
	unsigned int pad[2];
} __attribute__ ((packed));

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

#define COMPONENT_MAGIC "LARCHIVE"

struct cbfs_file {
	char magic[8];
	unsigned int len;
	unsigned int type;
	unsigned int checksum;
	unsigned int offset;
} __attribute__ ((packed));

/*** Component sub-headers ***/

/* Following are component sub-headers for the "standard"
   component types */

/** This is the sub-header for stage components.  Stages are
    loaded by coreboot during the normal boot process */

struct cbfs_stage {
	unsigned int compression;  /** Compression type */
	unsigned long long entry;  /** entry point */
	unsigned long long load;   /** Where to load in memory */
	unsigned int len;	   /** length of data to load */
	unsigned int memlen;	   /** total length of object in memory */
} __attribute__ ((packed));

/** this is the sub-header for payload components.  Payloads
    are loaded by coreboot at the end of the boot process */

struct cbfs_payload_segment {
	unsigned int type;
	unsigned int compression;
	unsigned int offset;
	unsigned long long load_addr;
	unsigned int len;
	unsigned int mem_len;
} __attribute__ ((packed));

struct cbfs_payload {
	struct cbfs_payload_segment segments;
};

#define PAYLOAD_SEGMENT_CODE   0x45444F43
#define PAYLOAD_SEGMENT_DATA   0x41544144
#define PAYLOAD_SEGMENT_BSS    0x20535342
#define PAYLOAD_SEGMENT_PARAMS 0x41524150
#define PAYLOAD_SEGMENT_ENTRY  0x52544E45

#define CBFS_NAME(_c) (((unsigned char *) (_c)) + sizeof(struct cbfs_file))

#endif
