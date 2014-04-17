/*
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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

#ifndef __CBFS_H
#define __CBFS_H

#include <stdint.h>

/* create a magic number in host-byte order.
 * b3 is the high order byte.
 * in the coreboot tools, we go with the 32-bit
 * magic number convention.
 * This was an inline func but that breaks anything
 * that uses it in a case statement.
 */

#define makemagic(b3, b2, b1, b0)\
	(((b3)<<24) | ((b2) << 16) | ((b1) << 8) | (b0))

#define CBFS_HEADER_MAGIC  0x4F524243
#define CBFS_HEADPTR_ADDR_X86 0xFFFFFFFC
#define CBFS_HEADER_VERSION1 0x31313131
#define CBFS_HEADER_VERSION2 0x31313132
#define CBFS_HEADER_VERSION  CBFS_HEADER_VERSION2

struct cbfs_header {
	uint32_t magic;
	uint32_t version;
	uint32_t romsize;
	uint32_t bootblocksize;
	uint32_t align;
	uint32_t offset;
	uint32_t architecture;	/* Version 2 */
	uint32_t pad[1];
} __attribute__ ((packed));

#define CBFS_ARCHITECTURE_UNKNOWN  0xFFFFFFFF
#define CBFS_ARCHITECTURE_X86      0x00000001
#define CBFS_ARCHITECTURE_ARMV7    0x00000010

#define CBFS_FILE_MAGIC "LARCHIVE"

struct cbfs_file {
	uint8_t magic[8];
	uint32_t len;
	uint32_t type;
	uint32_t checksum;
	uint32_t offset;
} __attribute__ ((packed));

struct cbfs_stage {
	uint32_t compression;
	uint64_t entry;
	uint64_t load;
	uint32_t len;
	uint32_t memlen;
} __attribute__ ((packed));

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
} __attribute__ ((packed));

struct cbfs_payload {
	struct cbfs_payload_segment segments;
} __attribute__ ((packed));

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define CBFS_COMPONENT_STAGE      0x10
#define CBFS_COMPONENT_PAYLOAD    0x20
#define CBFS_COMPONENT_OPTIONROM  0x30
#define CBFS_COMPONENT_BOOTSPLASH 0x40
#define CBFS_COMPONENT_RAW        0x50
#define CBFS_COMPONENT_VSA        0x51
#define CBFS_COMPONENT_MBI        0x52
#define CBFS_COMPONENT_MICROCODE  0x53
#define CBFS_COMPONENT_CMOS_DEFAULT 0xaa
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

#define CBFS_NAME(_c) (((char *) (_c)) + sizeof(struct cbfs_file))
#define CBFS_SUBHEADER(_p) ( (void *) ((((uint8_t *) (_p)) + ntohl((_p)->offset))) )
/* cbfs_image.c */
uint32_t get_cbfs_entry_type(const char *name, uint32_t default_value);
const char *get_cbfs_entry_type_name(uint32_t type);
uint32_t get_cbfs_compression(const char *name, uint32_t unknown);

/* common.c */
void cbfs_file_get_header(struct buffer *buf, struct cbfs_file *file);

/* cbfs-mkpayload.c */
void xdr_segs(struct buffer *output,
	      struct cbfs_payload_segment *segs, int nseg);

#endif
