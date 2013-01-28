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

#define PAYLOAD_SEGMENT_CODE	0x45444F43
#define PAYLOAD_SEGMENT_DATA	0x41544144
#define PAYLOAD_SEGMENT_BSS	0x20535342
#define PAYLOAD_SEGMENT_PARAMS	0x41524150
#define PAYLOAD_SEGMENT_ENTRY	0x52544E45

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

int cbfs_file_header(unsigned long physaddr);
#define CBFS_NAME(_c) (((char *) (_c)) + sizeof(struct cbfs_file))
#define CBFS_SUBHEADER(_p) ( (void *) ((((uint8_t *) (_p)) + ntohl((_p)->offset))) )

struct cbfs_file *cbfs_create_empty_file(uint32_t physaddr, uint32_t size);

#endif
